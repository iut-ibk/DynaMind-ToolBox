/**
 * @file
 * @author  Gregor Burger <burger.gregor@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2012  Gregor Burger
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "dmviewerwindow.h"
#include "ui_dmviewerwindow.h"
#include "dmaddlayerdialog.h"
#include "dmlayer.h"

namespace DM {

ViewerWindow::ViewerWindow(System *system, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ViewerWindow), system(system) {
    ui->setupUi(this);
    
    delete ui->color_bar_widget;
    ui->color_bar_widget = new ColorBarWidget(this, ui->viewer);
    
    ui->color_bar_widget->setObjectName(QString::fromUtf8("color_bar_widget"));
    QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(ui->color_bar_widget->sizePolicy().hasHeightForWidth());
    ui->color_bar_widget->setSizePolicy(sizePolicy);
    ui->color_bar_widget->setMinimumSize(QSize(0, 30));

    ui->verticalLayout->insertWidget(ui->verticalLayout->count()-2, ui->color_bar_widget);
    
    ui->viewer->setSystem(system);
    this->addAction(ui->actionAdd_Layer);
    this->addAction(ui->actionReset_View);
    offsetChanged();
    timer.setInterval(40);
    timer.setSingleShot(false);
    QObject::connect(&timer, SIGNAL(timeout()), this, SLOT(timerShot()));

	// buttons
	QObject::connect(ui->addLayer, SIGNAL(pressed()), ui->actionAdd_Layer, SLOT(trigger()));
	// as it has no function yet, we disable rm layer
	ui->removeLayer->setDisabled(true);
}

ViewerWindow::~ViewerWindow() {
    delete ui;
}

void ViewerWindow::addLayer(Layer *l, bool overdraw) {
    if (!l) return;
    ui->viewer->addLayer(l, overdraw);
    
    if (overdraw) return;
    
    QListWidgetItem *item = new QListWidgetItem(ui->layer_listWidget);
    item->setFlags(item->flags() | Qt::ItemIsSelectable);
    item->setCheckState(Qt::Checked);
    QStringList text;
    text << QString::fromStdString(l->getView().getName());
    if (l->getAttribute() != "") {
        text << QString::fromStdString(l->getAttribute());
    }
    item->setText(text.join(":"));
    
}

void ViewerWindow::on_actionAdd_Layer_triggered() 
{
	if(!system)
	{
		DM::Logger(Error) << "no data at port";
		return;
	}

    AddLayerDialog dialog(system, this);
    if (dialog.exec()) {
        Layer *l = dialog.getLayer(ui->viewer);
        if (!l) return;
        addLayer(l, dialog.isOverdrawLayer());
        QStringList attr_names = dialog.getAttributeVectorNames();
        if (attr_names.empty())
            return;
        ui->playButton->setEnabled(true);
        ui->names->setEnabled(true);
        ui->names->addItems(attr_names);
    }
}


void ViewerWindow::offsetChanged() {
    ui->viewer->setLayerOffset(ui->x_layerOffset->value(),
                               ui->y_layerOffset->value(),
                               ui->z_layerOffset->value());
}

void ViewerWindow::on_x_layerOffset_valueChanged(double ) {
    offsetChanged();
}

void ViewerWindow::on_y_layerOffset_valueChanged(double ) {
    offsetChanged();
}

void ViewerWindow::on_z_layerOffset_valueChanged(double ) {
    offsetChanged();
}

void ViewerWindow::on_actionReset_View_triggered() {
    ui->viewer->showEntireScene();
}

void ViewerWindow::on_names_currentIndexChanged(int name) {
    ui->viewer->setAttributeVectorName(name);
}

void ViewerWindow::on_playButton_clicked() {
    if (timer.isActive()) {
        timer.stop();
        ui->playButton->setIcon(QIcon::fromTheme("player_play"));
    } else {
        timer.start();
        ui->playButton->setIcon(QIcon::fromTheme("player_pause"));
    }
}

void ViewerWindow::on_layer_listWidget_itemChanged(QListWidgetItem *item) {
    int i = ui->layer_listWidget->row(item);
    ui->viewer->getLayer(i)->setEnabled(item->checkState() == Qt::Checked);
    ui->viewer->updateGL();
}

void ViewerWindow::on_layer_listWidget_currentRowChanged(int row) {
    if (row < 0) return;
    Layer *layer = ui->viewer->getLayer(row);
	GLuint tex = layer->getColorInterpretation();
	ui->color_bar_widget->setTexture(tex);
	if(tex || layer->getHeightInterpretation() > 0)
	{
		ui->min->setText(QString("%1").arg(layer->getViewMetaData().attr_min));
		ui->max->setText(QString("%1").arg(layer->getViewMetaData().attr_max));
		ui->min->setEnabled(true);
		ui->max->setEnabled(true);
	}
	else
	{
		ui->min->setText("");
		ui->max->setText("");
		ui->min->setEnabled(false);
		ui->max->setEnabled(false);
	}
}

void ViewerWindow::timerShot() {
    ui->names->setCurrentIndex((ui->names->currentIndex()+1) % ui->names->count());
}

void ViewerWindow::addLayers(std::list<LayerSpec> specs) {
    foreach(LayerSpec spec, specs) {
        addLayerFromSpec(spec);
    }
}

void ViewerWindow::addLayerFromSpec(LayerSpec spec) {
    DM::View v;
    bool found = false;
    foreach (v, system->getViews()) {
        if (v.getName() == spec.view) {
            found = true;
            break;
        }
    }
    Q_ASSERT(found);
    ui->viewer->makeCurrent();
    Layer *l = new Layer(system, v, spec.attribute);
    l->setHeightInterpretation(spec.height);
    l->setColorInterpretation(get_color_ramp(spec.ramp, l->LayerColor) );
    addLayer(l);
}

}
