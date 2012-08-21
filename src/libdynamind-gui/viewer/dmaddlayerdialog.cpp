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

#include "dmaddlayerdialog.h"
#include "ui_dmaddlayerdialog.h"
#include <QColorDialog>
#include <QLabel>
#include <QTreeWidgetItem>
#include <QGroupBox>

#include "dmattribute.h"
#include "dmlayer.h"
#include "dmviewer.h"
#include "dmlogger.h"

namespace DM {

AddLayerDialog::AddLayerDialog(DM::System *system, QWidget *parent) :
    QDialog(parent), system(system),
    ui(new Ui::AddLayerDialog) {
    ui->setupUi(this);
    start = Qt::white;
    stop = Qt::black;
    foreach (DM::View v, system->getViews()) {
        QStringList strings;
        strings << QString::fromStdString(v.getName());
        
        switch (v.getType()) {
            case DM::FACE:
                strings << "Face";
                break;
            case DM::EDGE:
                strings << "Edge";
                break;
            case DM::NODE:
                strings << "Node";
                break;
            default:
                continue;
        }
        
        QTreeWidgetItem *item = new QTreeWidgetItem(strings);
        ui->viewList->addTopLevelItem(item);
    }
    
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

AddLayerDialog::~AddLayerDialog() {
    delete ui;
}

GLuint makeColorRampTexture(DM::Viewer *v, QColor start, QColor stop) {
    v->makeCurrent();
    GLuint texture;
    QImage p(100, 10, QImage::Format_RGB32);
    QPainter painter;
    double h_inc = (stop.hueF() - start.hueF()) / 100.0;
    double s_inc = (stop.saturationF() - start.saturationF()) / 100.0;
    double v_inc = (stop.valueF() - start.valueF()) / 100.0;
    
    painter.begin(&p);
    for (int i = 0; i < 100; i++) {
        double h = start.hueF() + (i*h_inc);
        double s = start.saturationF() + (i*s_inc);
        double v = start.valueF() + (i*v_inc);
                
        QColor c = QColor::fromHsvF(h, s, v);
        painter.setPen(c);
        painter.drawRect(i, 0, 1, 10);
    }
    painter.end();
    
    glEnable(GL_TEXTURE_2D);
    texture = v->bindTexture(p, GL_TEXTURE_2D, GL_RGB);
    return texture;
}

DM::Layer *AddLayerDialog::getLayer(DM::Viewer *v) {
    if (!view)
        return 0;
    DM::Layer *l = new DM::Layer(system, *view, attribute);
    if (ui->colorCheckBox->isChecked()) {
        l->setColorInterpretation(makeColorRampTexture(v, start.toHsv(), stop.toHsv()));
    }
    if (ui->heightCheckBox->isChecked()) {
        l->setHeightInterpretation(ui->heightSpinBox->value());
    }
    return l;
}

QStringList AddLayerDialog::getAttributeVectorNames() const {
    if (attribute == "")
        return QStringList();
    
    if (!system->getUUIDsOfComponentsInView(*view).size())
        return QStringList();
    
    std::string uuid = system->getUUIDsOfComponentsInView(*view)[0];
    Attribute *attr = system->getComponent(uuid)->getAttribute(attribute);
    if (attr->getType() == Attribute::DOUBLEVECTOR) {
        int len = attr->getDoubleVector().size();
        QStringList list;
        for (int i = 0; i < len; i++) {
            list << QString("%1").arg(i);
        }
    }
    if (attr->getType() == Attribute::TIMESERIES) {
        QStringList list;
        foreach(std::string s, attr->getStringVector()) {
            list << QString::fromStdString(s);
        }
        return list;
    }
    return QStringList();
}

void AddLayerDialog::on_color_start_button_clicked() {
    start = QColorDialog::getColor(start, this);
    ui->color_start_label->setStyleSheet("background-color: " + start.name());
}

void AddLayerDialog::on_color_stop_button_clicked() {
    stop = QColorDialog::getColor(stop, this);
    ui->color_stop_label->setStyleSheet("background-color: " + stop.name());
}

void AddLayerDialog::on_viewList_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *) {
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(current);
    ui->attributeList->clear();
    view = system->getViewDefinition(current->text(0).toStdString());
    
    if (!system->getUUIDsOfComponentsInView(*view).size()) return;
    
    std::string uuid = system->getUUIDsOfComponentsInView(*view)[0];
    
    QMap<std::string, DM::Attribute*> attributes(system->getComponent(uuid)->getAllAttributes());
    
    foreach(std::string key, attributes.keys()) {
        DM::Attribute *attr = attributes[key];
        if (! (attr->getType() == Attribute::DOUBLE
            || attr->getType() == Attribute::DOUBLEVECTOR
            || attr->getType() == Attribute::TIMESERIES)) {
            continue;
        }
        
        QStringList strings;
        strings << QString::fromStdString(key);
        strings << attr->getTypeName();
        QTreeWidgetItem *item = new QTreeWidgetItem(strings);
        ui->attributeList->addTopLevelItem(item);
    }
}

void AddLayerDialog::on_attributeList_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous) {
    attribute = current ? current->text(0).toStdString() : "";
    
    ui->interpreteGroup->setEnabled(current);
}

}
