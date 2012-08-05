/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2012  Christian Urich
 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "guiattributecalculator.h"
#include "ui_guiattributecalculator.h"

GUIAttributeCalculator::GUIAttributeCalculator(DM::Module * m, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GUIAttributeCalculator)
{
    ui->setupUi(this);
    this->attrcalc = (AttributeCalculator*) m;

    if (!this->attrcalc->getSystemIn())
        return;

    std::vector<DM::View> views= this->attrcalc->getSystemIn()->getViews();
    this->ui->treeViews->setColumnCount(2);
    QTreeWidgetItem * headerItem = this->ui->treeViews->headerItem();
    headerItem->setText(0, "Data Stream");
    headerItem->setText(1, "Type");

    foreach (DM::View v, views) {
        if (v.getName().compare("dummy") == 0)
            continue;
        QTreeWidgetItem * item_view = new QTreeWidgetItem();
        item_view->setText(0, QString::fromStdString(v.getName()));
        int type = v.getType();

        if (type == DM::NODE)
            item_view->setText(1, "Nodes");
        if (type == DM::EDGE)
            item_view->setText(1, "Edges");
        if (type == DM::FACE)
            item_view->setText(1, "Faces");
        if (type == DM::SYSTEM)
            item_view->setText(1, "Systems");
        if (type == DM::RASTERDATA)
            item_view->setText(1, "Raster Data");

        foreach (std::string s, v.getReadAttributes()) {
            QTreeWidgetItem * item_attribute = new QTreeWidgetItem();
            item_attribute->setText(0, QString::fromStdString(s));
            item_view->addChild(item_attribute);
        }
        foreach (std::string s, v.getWriteAttributes()) {
            QTreeWidgetItem * item_attribute = new QTreeWidgetItem();
            item_attribute->setText(0, QString::fromStdString(s));
            item_view->addChild(item_attribute);
        }
        this->ui->treeViews->addTopLevelItem(item_view);
        this->ui->treeViews->expandItem(item_view);
    }
}




GUIAttributeCalculator::~GUIAttributeCalculator()
{
    delete ui;
}
