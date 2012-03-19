/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
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
#include "guicellularautomata.h"
#include "ui_guicellularautomata.h"
#include <dmmodule.h>
#include <QInputDialog>
#include <guicellularautomata_neighbourhood.h>
#include <guiequationeditor.h>
#include <QTableWidgetSelectionRange>
#include <dm.h>
#include <cellularautomata.h>

using namespace DM;
bool GUICellularAutomata::checkIfFromOutSide(QString name) {
    QString s2 = "DoubleIn_" + name;
    std::map<std::string, double> doublemap = m->getParameter<std::map<std::string, double> >("InputDouble");
    for (std::map<std::string, double>::iterator it = doublemap.begin(); it != doublemap.end(); ++it) {
        if (s2.toStdString().compare(it->first) == 0)
            return true;
    }
    return false;

}

GUICellularAutomata::GUICellularAutomata( DM::Module * m, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GUICellularAutomata)
{
    this->m = (CellularAutomata*) m;
    ui->setupUi(this);
    ui->lineEdit_Height->setText( QString::fromStdString(m->getParameterAsString("Height")) );
    ui->lineEdit_Width->setText( QString::fromStdString(m->getParameterAsString("Width")) );
    ui->lineEdit_CellSize->setText( QString::fromStdString(m->getParameterAsString("CellSize")) );
    ui->lineEdit_resultName->setText( QString::fromStdString(m->getParameterAsString("NameOfOutput")) );
    std::map<std::string, std::vector<DM::View> > views =  m->getViews();

    foreach (std::string s, this->m->getLandscapes())
        ui->listWidget_landscapes->addItem(QString::fromStdString(s));




    ui->lineEdit_descision->setText(QString::fromStdString(m->getParameterAsString("Desicion")));
    ui->spinBox_Steps->setValue(m->getParameter<int>("Steps"));

    QObject::connect(ui->pushButton_addLandscape, SIGNAL(clicked()), this, SLOT(addRasterData()));
    QObject::connect(ui->pushButton_addNeigh, SIGNAL(clicked()), this, SLOT(addVariable()));
    QObject::connect(ui->pushButton_formula, SIGNAL(clicked()), this, SLOT(addFormula()));
    QObject::connect(ui->pushButton_edit, SIGNAL(clicked()), this, SLOT(editRule()));
    QObject::connect(ui->pushButton_expression, SIGNAL(clicked()), this, SLOT(addExpression()));
    QObject::connect(ui->checkBox_Height, SIGNAL(clicked()), this, SLOT(fromOutSide()));
    QObject::connect(ui->checkBox_Width, SIGNAL(clicked()), this, SLOT(fromOutSide()));
    QObject::connect(ui->checkBox_CellSize, SIGNAL(clicked()), this, SLOT(fromOutSide()));


    QStringList headers;
    headers << "Name" << "Landscape" << "Type";
    ui->tableWidget_neighs->setHorizontalHeaderLabels(headers);
    ui->tableWidget_neighs->setColumnWidth (0,320);
    ui->tableWidget_neighs->setColumnWidth (1,320);
    ui->tableWidget_neighs->setColumnWidth (2,220);

    headers.clear();
    headers << "Name" << "Rules";
    ui->tableWidget_rules->setHorizontalHeaderLabels(headers);
    ui->tableWidget_rules->setColumnWidth (0,320);
    ui->tableWidget_rules->setColumnWidth (1,540);

    updateEntries();



}
void GUICellularAutomata::fromOutSide() {
    std::cout << "From Outside" << std::endl;
    QCheckBox * b = (QCheckBox *)QObject::sender();
    QString s =b->objectName();
    QStringList s1 = s.split("_");
    QString name;
    name = "DoubleIn_" + s1[1];
    if (b->checkState ()) {
        //this->m->appendToUserDefinedParameter("InputDouble",name.toStdString());
    } else {
        //this->m->de("InputDouble","DoubleIn_Width");
    }
}

void GUICellularAutomata::updateEntries() {

    std::map<std::string, std::string> neighs = m->getParameter< std::map<std::string, std::string> >("Neighs");
    for (std::map<std::string, std::string>::iterator it = neighs.begin(); it != neighs.end(); ++it) {
        QString name = QString::fromStdString(it->first);
        QString value = QString::fromStdString(it->second);
        QStringList valuelist = value.split("+|+");
        if (variables.indexOf(name) < 0) {
            variables << name;
            ui->tableWidget_neighs->setRowCount( ui->tableWidget_neighs->rowCount()+1);

            QTableWidgetItem * item = new QTableWidgetItem(name);
            ui->tableWidget_neighs->setItem(ui->tableWidget_neighs->rowCount()-1,0, item);
            item = new QTableWidgetItem(valuelist[0]);
            ui->tableWidget_neighs->setItem(ui->tableWidget_neighs->rowCount()-1,1, item);
            item = new QTableWidgetItem(valuelist[1]);
            ui->tableWidget_neighs->setItem(ui->tableWidget_neighs->rowCount()-1,2, item);
            if (valuelist[1] == "CurrentValue") {
                this->values << name;
            } else {
                this->vectors << name;
            }
        }
    }

    std::map<std::string, std::string> r = m->getParameter< std::map<std::string, std::string> >("Rules");
    ui->tableWidget_rules->clear();
    ui->tableWidget_rules->setRowCount(0);
    rules.clear();
    for (std::map<std::string, std::string>::iterator it = r.begin(); it != r.end(); ++it) {
        QString name = QString::fromStdString(it->first);
        QString value = QString::fromStdString(it->second);
        if (rules.indexOf(name) < 0) {
            rules << name;
            ui->tableWidget_rules->setRowCount( ui->tableWidget_rules->rowCount()+1);
            QTableWidgetItem * item = new QTableWidgetItem(name);
            ui->tableWidget_rules->setItem(ui->tableWidget_rules->rowCount()-1,0, item);
            item = new QTableWidgetItem(value);
            ui->tableWidget_rules->setItem(ui->tableWidget_rules->rowCount()-1,1, item);
        }
    }
}

void GUICellularAutomata::addRasterData() {
    bool ok;
    std::cout << "AddRasterData" << std::endl;
    QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                         tr("Name:"), QLineEdit::Normal, "" , &ok);
    if (ok && !text.isEmpty()) {
        m->addLandscape(text.toStdString());
        ui->listWidget_landscapes->addItem(text);
    }


}
void GUICellularAutomata::addVariable() {
    GUICellularAutomata_Neighbourhood * dlg = new GUICellularAutomata_Neighbourhood(this, this);
    QObject::connect(dlg, SIGNAL(valuesChanged()), this, SLOT(updateEntries()));
    dlg->show();

}
void GUICellularAutomata::addFormula() {
    GUIEquationEditor * dlg = new GUIEquationEditor(values, vectors, this);

    QObject::connect(dlg, SIGNAL(values(QStringList)), this, SLOT(addRule(QStringList)));
    dlg->show();
}

GUICellularAutomata::~GUICellularAutomata()
{
    delete ui;
}
void GUICellularAutomata::addExpression() {
    GUIEquationEditor * dlg = new GUIEquationEditor(this->rules, QStringList(), this);
    QObject::connect(dlg, SIGNAL(values(QStringList)), this, SLOT(addExpressiontoVIBe(QStringList)));
    dlg->show();
}

void GUICellularAutomata::editRule() {
    GUIEquationEditor * dlg = new GUIEquationEditor(values, vectors, this);
    int rowcount = ui->tableWidget_rules->currentRow();
    if (rowcount == -1)
        return;
    dlg->setName(ui->tableWidget_rules->item(rowcount,0)->text());
    dlg->setFormula(ui->tableWidget_rules->item(rowcount,1)->text());

    QObject::connect(dlg, SIGNAL(values(QStringList)), this, SLOT(addRule(QStringList)));
    dlg->show();
}


void GUICellularAutomata::addExpressiontoVIBe(QStringList list) {
    m->setParameterValue("Desicion", list[1].toStdString());
    ui->lineEdit_descision->setText(list[1]);
}
void GUICellularAutomata::addRule(QStringList list) {
    std::map<std::string, std::string> neighs = m->getParameter< std::map<std::string, std::string> >("Rules");

    std::string name = list[0].toStdString();
    neighs[name] =list[1].toStdString();
    std::cout << "Update" << name << list[1].toStdString() << std::endl;
    m->setParameterNative< std::map<std::string, std::string> >("Rules", neighs);

    this->updateEntries();
}




void GUICellularAutomata::accept() {
    this->m->setParameterValue("Height", ui->lineEdit_Height->text().toStdString());
    this->m->setParameterValue("Width", ui->lineEdit_Width->text().toStdString());
    this->m->setParameterValue("CellSize", ui->lineEdit_CellSize->text().toStdString());
    this->m->setParameterValue("NameOfOutput", ui->lineEdit_resultName->text().toStdString());
    QDialog::accept();
}
