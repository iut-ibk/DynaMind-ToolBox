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
    ui->viewer->setSystem(system);
    this->addAction(ui->actionAdd_Layer);
    this->addAction(ui->actionReset_View);
    offsetChanged();
    timer.setInterval(40);
    timer.setSingleShot(false);
    QObject::connect(&timer, SIGNAL(timeout()), this, SLOT(timerShot()));
}

ViewerWindow::~ViewerWindow() {
    delete ui;
}

void ViewerWindow::on_actionAdd_Layer_triggered() {
    AddLayerDialog dialog(system, this);
    if (dialog.exec()) {
        Layer *l = dialog.getLayer(ui->viewer);
        if (!l) return;
        ui->viewer->addLayer(l);
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

void ViewerWindow::timerShot() {
    ui->names->setCurrentIndex((ui->names->currentIndex()+1) % ui->names->count());
}

}
