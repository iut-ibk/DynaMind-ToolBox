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
#include <guicellularautomata_selectlandscape.h>

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
    ui->lineEdit_OffsetX->setText( QString::fromStdString(m->getParameterAsString("OffsetX")) );
    ui->lineEdit_OffsetY->setText( QString::fromStdString(m->getParameterAsString("OffsetY")) );
    ui->lineEdit_resultName->setText( QString::fromStdString(m->getParameterAsString("NameOfOutput")) );

    //std::map<std::string, std::vector<DM::View> > views =  m->getViews();

    ui->comboBox_nameOfExisting->clear();

    ui->comboBox_nameOfExisting->addItem("user defined");
    std::vector<std::string> landscapes =  this->m->getLandscapesInStream();
    foreach(std::string l, landscapes)
        ui->comboBox_nameOfExisting->addItem(QString::fromStdString(l));
    //Choose Box
    std::string n_dim = m->getParameter<std::string>("DimensionOfExisting");
    int index = ui->comboBox_nameOfExisting->findText(QString::fromStdString(n_dim));
    if (index > -1) ui->comboBox_nameOfExisting->setCurrentIndex(index);
    else ui->comboBox_nameOfExisting->setCurrentIndex(0);

    ui->checkBox_dimesionFromOutside->setChecked(m->getParameter<bool>("appendToStream"));

    foreach (std::string s, this->m->getLandscapes())
        ui->listWidget_landscapes->addItem(QString::fromStdString(s));

    ui->lineEdit_descision->setText(QString::fromStdString(m->getParameterAsString("Desicion")));

    QObject::connect(ui->pushButton_addLandscape, SIGNAL(clicked()), this, SLOT(addRasterData()));
    QObject::connect(ui->pushButton_addNeigh, SIGNAL(clicked()), this, SLOT(addVariable()));
    QObject::connect(ui->pushButton_formula, SIGNAL(clicked()), this, SLOT(addFormula()));
    QObject::connect(ui->pushButton_edit, SIGNAL(clicked()), this, SLOT(editRule()));
    QObject::connect(ui->pushButton_expression, SIGNAL(clicked()), this, SLOT(addExpression()));

    QStringList headers;
    headers << "Name" << "Landscape" << "Type";
    ui->tableWidget_neighs->clearContents();
    ui->tableWidget_neighs->setRowCount(0);
    ui->tableWidget_neighs->setHorizontalHeaderLabels(headers);
    ui->tableWidget_neighs->setColumnWidth (0,70);
    ui->tableWidget_neighs->setColumnWidth (1,100);
    ui->tableWidget_neighs->setColumnWidth (2,100);

    headers.clear();
    headers << "Name" << "Rule";
    ui->tableWidget_rules->setHorizontalHeaderLabels(headers);
    ui->tableWidget_rules->setColumnWidth (0,70);
    ui->tableWidget_rules->setColumnWidth (1,200);

    updateEntries();

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
    ui->tableWidget_rules->clearContents();
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

    std::vector<std::string> list = this->m->getLandscapesInStream();
    QStringList lcs;
    foreach (std::string l, list) lcs << QString::fromStdString(l);
    GUICellularAutomata_SelectLandscape * dlg = new GUICellularAutomata_SelectLandscape(lcs, this);
    QObject::connect(dlg, SIGNAL(selected(QString)), this, SLOT(addLandscape(QString)));
    dlg->show();
}

void GUICellularAutomata::addLandscape(QString text)
{
    if (!text.isEmpty()) {
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

    dlg->setFormula(ui->lineEdit_descision->text());
    dlg->hideName();
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
    m->setParameterNative< std::map<std::string, std::string> >("Rules", neighs);

    this->updateEntries();
}

void GUICellularAutomata::accept() {
    this->m->setParameterValue("Height", ui->lineEdit_Height->text().toStdString());
    this->m->setParameterValue("Width", ui->lineEdit_Width->text().toStdString());
    this->m->setParameterValue("CellSize", ui->lineEdit_CellSize->text().toStdString());
    this->m->setParameterValue("OffsetX", ui->lineEdit_OffsetX->text().toStdString());
    this->m->setParameterValue("OffsetY", ui->lineEdit_OffsetY->text().toStdString());
    this->m->setParameterValue("NameOfOutput", ui->lineEdit_resultName->text().toStdString());
    bool ischecked = ui->checkBox_dimesionFromOutside->isChecked();
    this->m->setParameterNative<bool>("appendToStream",ischecked);
    this->m->setParameterNative<std::string>("DimensionOfExisting",ui->comboBox_nameOfExisting->currentText().toStdString());
    this->m->setParameterNative<std::string>("Desicion", ui->lineEdit_descision->text().toStdString());
	m->init();
    QDialog::accept();
}

void GUICellularAutomata::on_pushButton_remove_landscape_clicked()
{
    QListWidgetItem * item = this->ui->listWidget_landscapes->currentItem();
    if (!item) return;
    Logger(Debug) << "remove";
    this->m->removeLandscape( this->ui->listWidget_landscapes->currentItem()->text().toStdString() );
    delete item;
}

void GUICellularAutomata::on_pushButton_remove_neigh_clicked()
{
    QTableWidgetItem * item =  this->ui->tableWidget_neighs->itemAt(0,ui->tableWidget_neighs->currentRow());

    if (!item) return;
    m->removeNeighboorhood(item->text().toStdString());

    this->ui->tableWidget_neighs->removeRow(item->row());
}

void GUICellularAutomata::on_pushButton_remove_rules_clicked()
{
    QTableWidgetItem * item =  this->ui->tableWidget_rules->itemAt(0,ui->tableWidget_rules->currentRow());

    if (!item) return;
    m->removeRule(item->text().toStdString());

    this->ui->tableWidget_rules->removeRow(item->row());
}
