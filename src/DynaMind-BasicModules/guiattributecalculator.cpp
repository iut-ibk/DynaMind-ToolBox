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


    ui->lineExpression->setText(QString::fromStdString( this->attrcalc->getParameterAsString("equation")));
    ui->lineEditAttribute->setText(QString::fromStdString( this->attrcalc->getParameterAsString("nameOfNewAttribute")));

    std::vector<DM::View> views= this->attrcalc->getSystemIn()->getViews();
    foreach (DM::View v, views) {
        ui->comboView->addItem(QString::fromStdString(v.getName()));
    }
    if (views.size() > 0) {
        std::string nameOfBaseView = this->attrcalc->getParameter<std::string>("NameOfBaseView");
        if (nameOfBaseView.empty()) {
            viewName =QString::fromStdString(views[0].getName());
        } else {
            int index = ui->comboView->findText(QString::fromStdString(nameOfBaseView));
            if (index != -1) {
                ui->comboView->setCurrentIndex(index);
            }
        }
        this->updateAttributeView();
    }


    //CreateVaraibles List
    std::map<std::string, std::string> variables = this->attrcalc->getParameter<std::map<std::string, std::string> >("variablesMap");

    for (std::map<std::string, std::string>::iterator it = variables.begin(); it != variables.end(); ++it) {

        ui->varaibleTable->setRowCount( ui->varaibleTable->rowCount()+1);
        QTableWidgetItem * item = new QTableWidgetItem(QString::fromStdString(it->first));
        ui->varaibleTable->setItem(ui->varaibleTable->rowCount()-1,0, item);
        item = new QTableWidgetItem(QString::fromStdString(it->second));
        ui->varaibleTable->setItem(ui->varaibleTable->rowCount()-1,1, item);
    }

}


GUIAttributeCalculator::~GUIAttributeCalculator()
{
    delete ui;
}

void GUIAttributeCalculator::createTreeViewEntries(QTreeWidgetItem * root_port, std::string viewname) {

    std::vector<std::string> views= this->attrcalc->getSystemIn()->getNamesOfViews();
    foreach (std::string vn, views) {
        DM::View * v = this->attrcalc->getSystemIn()->getViewDefinition(vn);
        if (v->getName().compare("dummy") == 0)
            continue;
        if (v->getName().compare(viewname) != 0)
            continue;

        DM::Component * c = this->attrcalc->getSystemIn()->getComponent(v->getIdOfDummyComponent());
        if (c == 0) {
            continue;
        }
        std::map<std::string,DM::Attribute*> attributes = c->getAllAttributes();
        for (std::map<std::string,DM::Attribute*>::const_iterator it  = attributes.begin(); it != attributes.end(); ++it) {
            DM::Logger(DM::Debug) << it->first;
            Attribute * attr = it->second;
            QTreeWidgetItem * item_attribute = new QTreeWidgetItem();
            item_attribute->setText(0, QString::fromStdString(it->first));
            root_port->addChild(item_attribute);
            if (attr->getType() == LINK) {
                createTreeViewEntries(item_attribute, it->first);
            }
        }


    }
}

void GUIAttributeCalculator::updateAttributeView()
{

    std::vector<std::string> views= this->attrcalc->getSystemIn()->getNamesOfViews();

    ui->listAttributes->clear();
    if (viewName.isEmpty())
        return;
    foreach (std::string v, views) {
        if (v.compare("dummy") == 0)
            continue;
        if (v.compare(viewName.toStdString()) != 0)
            continue;
        QTreeWidgetItem * headerItem = this->ui->listAttributes->headerItem();
        headerItem->setText(0, "View");
        headerItem->setText(1, "Attribute");
        QTreeWidgetItem * root_port = new QTreeWidgetItem();
        this->ui->listAttributes->addTopLevelItem(root_port);
        root_port->setText(0, QString::fromStdString(v));
        this->createTreeViewEntries(root_port, v);

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

    std::vector<std::string> elements;
    elements.push_back(ui->listAttributes->currentItem()->text(0).toStdString());
    QTreeWidgetItem * parent = ui->listAttributes->currentItem()->parent();

    do {
        elements.push_back(parent->text(0).toStdString());
        QTreeWidgetItem * parent = parent->parent();
    } while (!parent);


    for (int i = elements.size()-1; i > -1; --i)
        elementName << "." <<elements[i];

    ui->varaibleTable->setRowCount( ui->varaibleTable->rowCount()+1);
    QTableWidgetItem * item = new QTableWidgetItem(QString::fromStdString(elementName.str()));
    ui->varaibleTable->setItem(ui->varaibleTable->rowCount()-1,0, item);
    item = new QTableWidgetItem(text);
    ui->varaibleTable->setItem(ui->varaibleTable->rowCount()-1,1, item);
}

void GUIAttributeCalculator::on_comboView_currentIndexChanged (int val)
{
    viewName = ui->comboView->currentText();
    this->attrcalc->setParameterValue("NameOfBaseView", viewName.toStdString());
    this->updateAttributeView();

}

void GUIAttributeCalculator::accept() {
    this->attrcalc->setParameterValue("NameOfBaseView", ui->comboView->currentText().toStdString());
    this->attrcalc->setParameterValue("equation", ui->lineExpression->text().toStdString());
    this->attrcalc->setParameterValue("nameOfNewAttribute", ui->lineEditAttribute->text().toStdString());


    int rows = ui->varaibleTable->rowCount();

    std::map<std::string, std::string> variables;
    for (int i = 0; i < rows; i++) {
        variables[ui->varaibleTable->item(i, 0)->text().toStdString()] = ui->varaibleTable->item(i, 1)->text().toStdString();

    }
    this->attrcalc->setParameterNative<std::map<std::string, std::string > >("variablesMap", variables);
    QDialog::accept();
}
