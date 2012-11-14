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
#include "dmcolorramp.h"

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
        case DM::COMPONENT:
            strings << "Component";
            break;
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
    QStringList color_ramp_names;
    for (int i = 0; i < LAST_COLOR; i++) {
        color_ramp_names << ColorRampNames[i];
    }
    ui->colorRamp->addItems(color_ramp_names);
}

AddLayerDialog::~AddLayerDialog() {
    delete ui;
}

DM::Layer *AddLayerDialog::getLayer(DM::Viewer *v) {
    if (!view)
        return 0;
    DM::Layer *l = new DM::Layer(system, *view, attribute, ui->checkBox3DObject->isChecked());
    if (ui->colorCheckBox->isChecked()) {
        v->makeCurrent();
        l->setColorInterpretation(get_color_ramp((ColorRamp)ui->colorRamp->currentIndex()));
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

bool AddLayerDialog::isOverdrawLayer() const {
    return ui->overdraw->isChecked();
}

void AddLayerDialog::on_viewList_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *) {
    ui->attributeList->clear();
    ui->overdraw->setEnabled(current);
    view = system->getViewDefinition(current->text(0).toStdString());

    std::string uuid =  view->getIdOfDummyComponent();
    std::vector<std::string> uuids = system->getUUIDsOfComponentsInView(*view);
    QMap<std::string, DM::Attribute::AttributeType> attrTypes;
    QMap<std::string, DM::Attribute*> attributes_tmp(system->getComponent(uuid)->getAllAttributes());
    QMap<std::string, DM::Attribute*> attributes;
    foreach (std::string k, attributes_tmp.keys()) {
        if (attributes_tmp[k]) {
            attributes[k] = attributes_tmp[k];
        }
    }
    foreach (std::string k, attributes.keys()) {
        attrTypes[k] = attributes[k]->getType();
    }
    
    foreach (std::string uuid,uuids) {
        QMap<std::string, DM::Attribute*> attrs(system->getComponent(uuid)->getAllAttributes());
        foreach(std::string k, attrs.keys()) {
            attrTypes[k] = attrs[k]->getType();
        }
        bool exists = true;
        foreach(std::string k, attrTypes.keys()) {
            if (attrTypes[k] == DM::Attribute::NOTYPE) {
                exists = false;
                continue;
            }
        }
        if (exists)
            break;
    }
    foreach (std::string k, attrTypes.keys()) {
        if (attributes.find(k) == attributes.end())
            continue;
        attributes[k]->setType(attrTypes[k]);
    }

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
