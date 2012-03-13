/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of VIBe2
 *
 * Copyright (C) 2011  Christian Urich
 
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

#include "guidynamicinout.h"
#include "ui_guidynamicinout.h"
#include <dynamicinout.h>
#include <dmmodule.h>
#include <dmview.h>
#include <dm.h>
#include "qtreewidget.h"
#include "qinputdialog.h"

GUIDynamicInOut::GUIDynamicInOut(DM::Module *m, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GUIDynamicInOut)
{


    ui->setupUi(this);
    this->module = (DynamicInOut*) m;

    std::vector<DM::View> views = module->getViews();

    QTreeWidget * tree = ui->treeWidget;

    QList<QTreeWidgetItem *> items;
    connect(ui->addAttribute, SIGNAL(pressed()), this, SLOT(addAttribute()));
    connect(ui->getAttribute, SIGNAL(pressed()), this, SLOT(getAttribute()));


    foreach (DM::View v, views) {
        QStringList entry;
        entry << QString::fromStdString(v.getName());
        if (v.getType() == DM::SYSTEM)
            entry << QString::fromStdString("System");
        if (v.getType() == DM::NODE)
            entry << QString::fromStdString("Node");
        if (v.getType() == DM::EDGE)
            entry << QString::fromStdString("Edge");
        QTreeWidgetItem * item = new QTreeWidgetItem((QTreeWidget*)0, entry);
        foreach (std::string s,  v.getReadAttributes()){
            QStringList entry1;
            entry1 << QString::fromStdString(s);
            entry1 << QString::fromStdString("Read");
            QTreeWidgetItem * item1 = new QTreeWidgetItem((QTreeWidget*)0, entry1);
            item->addChild(item1);
        }
        foreach (std::string s,  v.getWriteAttributes()){
            QStringList entry1;
            entry1 << QString::fromStdString(s);
            entry1 << QString::fromStdString("Write");
            QTreeWidgetItem * item1 = new QTreeWidgetItem((QTreeWidget*)0, entry1);
            item->addChild(item1);
        }
        items.append(item);
    }
    tree->insertTopLevelItems(0, items);
    tree->setColumnCount(2);



}

void GUIDynamicInOut::addAttribute() {
    bool ok;
    QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                         tr("Name"), QLineEdit::Normal,
                                         "", &ok);
    if (ok && !text.isEmpty())
        this->module->addAttribute(text.toStdString());
}

void GUIDynamicInOut::getAttribute() {
    bool ok;
    QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                         tr("Name"), QLineEdit::Normal,
                                         "", &ok);
    if (ok && !text.isEmpty())
        this->module->getAttribute(text.toStdString());
}

GUIDynamicInOut::~GUIDynamicInOut()
{
    delete ui;
}


