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
#include <QInputDialog>
#include <sstream>

GUIAttributeCalculator::GUIAttributeCalculator(DM::Module * m, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GUIAttributeCalculator)
{
    ui->setupUi(this);
    this->attrcalc = (AttributeCalculator*) m;

    if (!this->attrcalc->getSystemIn())
        return;


    QStringList headers;
    headers << "Name" << "Landscape";
    ui->varaibleTable->setHorizontalHeaderLabels(headers);
    ui->varaibleTable->setColumnWidth (0,320);
    ui->varaibleTable->setColumnWidth (1,320);
    ui->comboView->clear();

    std::vector<DM::View> views= this->attrcalc->getSystemIn()->getViews();
    foreach (DM::View v, views) {
        ui->comboView->addItem(QString::fromStdString(v.getName()));
    }
    if (views.size() > 0) {
        viewName =QString::fromStdString(views[0].getName());
    }

}


GUIAttributeCalculator::~GUIAttributeCalculator()
{
    delete ui;
}

void GUIAttributeCalculator::updateAttributeView()
{

    std::vector<DM::View> views= this->attrcalc->getSystemIn()->getViews();

    ui->listAttributes->clear();
    if (viewName.isEmpty())
        return;
    foreach (DM::View v, views) {
        if (v.getName().compare("dummy") == 0)
            continue;
        if (v.getName().compare(viewName.toStdString()) != 0)
            continue;

        foreach (std::string s, v.getReadAttributes()) {
            ui->listAttributes->addItem(QString::fromStdString(s));
        }
        foreach (std::string s, v.getWriteAttributes()) {
            ui->listAttributes->addItem(QString::fromStdString(s));
        }
    }

}

void GUIAttributeCalculator::on_addButton_clicked() {
    bool ok;
    QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                         tr("Varaible Name"), QLineEdit::Normal,
                                         "", &ok);
    if (!ok || text.isEmpty())
        return;

    std::stringstream elementName;

    if (ui->listAttributes->size().isNull())
        return;
    elementName << ui->comboView->currentText().toStdString();
    elementName << ".";

    elementName << ui->listAttributes->currentItem()->text().toStdString();

    ui->varaibleTable->setRowCount( ui->varaibleTable->rowCount()+1);

    QTableWidgetItem * item = new QTableWidgetItem(QString::fromStdString(elementName.str()));
    ui->varaibleTable->setItem(ui->varaibleTable->rowCount()-1,0, item);
    item = new QTableWidgetItem(text);
    ui->varaibleTable->setItem(ui->varaibleTable->rowCount()-1,1, item);
}

void GUIAttributeCalculator::on_comboView_currentIndexChanged (int val)
{
    viewName = ui->comboView->currentText();
    this->updateAttributeView();

}
