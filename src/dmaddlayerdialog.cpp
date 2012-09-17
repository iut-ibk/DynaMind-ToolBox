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
#include "glext.h"
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
}

AddLayerDialog::~AddLayerDialog() {
    delete ui;
}

GLuint makeColorRampTexture(DM::Viewer *v, QColor start, QColor stop) {
    v->makeCurrent();
    GLuint texture;
    glEnable(GL_TEXTURE_1D);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_1D, texture);
    
    GLubyte data[256][4];
    
    double h_inc = (stop.hueF() - start.hueF()) / 256.0;
    double s_inc = (stop.saturationF() - start.saturationF()) / 256.0;
    double v_inc = (stop.valueF() - start.valueF()) / 256.0;
    
    for (int i = 0; i < 256; i++) {
        double h = start.hueF() + (i*h_inc);
        double s = start.saturationF() + (i*s_inc);
        double v = start.valueF() + (i*v_inc);

        QColor c = QColor::fromHsvF(h, s, v);
        data[i][0] = c.red();
        data[i][1] = c.green();
        data[i][2] = c.blue();
        data[i][3] = c.alpha();
    }
    
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    Q_ASSERT(glGetError() == GL_NO_ERROR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    Q_ASSERT(glIsTexture(texture));
    return texture;
}

DM::Layer *AddLayerDialog::getLayer(DM::Viewer *v) {
    if (!view)
        return 0;
    DM::Layer *l = new DM::Layer(system, *view, attribute);
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

void AddLayerDialog::on_viewList_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *) {
    ui->attributeList->clear();
    view = system->getViewDefinition(current->text(0).toStdString());
    
    //if (!system->getUUIDsOfComponentsInView(*view).size()) return;
    


    std::string uuid =  view->getIdOfDummyComponent();
    std::vector<std::string> uuids = system->getUUIDsOfComponentsInView(*view);
    QMap<std::string, DM::Attribute::AttributeType> attrTypes;
    QMap<std::string, DM::Attribute*> attributes(system->getComponent(uuid)->getAllAttributes());

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
