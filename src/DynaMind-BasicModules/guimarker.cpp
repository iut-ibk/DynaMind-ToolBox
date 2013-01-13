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
#include "guimarker.h"
#include "ui_guimarker.h"
#include "marker.h"
#include "dmmodule.h"
#include <QInputDialog>
#include <guiequationeditor.h>

using namespace DM;


GUIMarker::GUIMarker(DM::Module * m, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GUIMarker)
{
    this->m = (Marker*) m;
    ui->setupUi(this);
    DM::System * sys = this->m->getSystemIn();
    std::vector<std::string> sys_in;
    if (sys != 0)
        sys_in = sys->getNamesOfViews();

    ui->comboBox->clear();

    foreach (std::string s, sys_in) {
        ui->comboBox->addItem(QString::fromStdString(s));
    }

    std::string nameofexview = this->m->getParameterAsString("Identifier");
    if (!nameofexview.empty()) {
        int index = ui->comboBox->findText(QString::fromStdString(nameofexview));
        ui->comboBox->setCurrentIndex(index);
    }

    if (ui->comboBox->count() == 0) {
        ui->comboBox->addItem("Connect Inport");
    }


    ui->lineEdit_Height->setText( QString::fromStdString(m->getParameterAsString("Height")) );
    ui->lineEdit_Width->setText( QString::fromStdString(m->getParameterAsString("Width")) );
    ui->lineEdit_CellSize->setText( QString::fromStdString(m->getParameterAsString("CellSize")) );
    ui->lineEdit_OffsetX->setText( QString::fromStdString(m->getParameterAsString("OffsetX")) );
    ui->lineEdit_OffsetY->setText( QString::fromStdString(m->getParameterAsString("OffsetY")) );

    ui->lineEdit_RExpression->setText( QString::fromStdString(m->getParameterAsString("RExpression")) );
    ui->lineEdit_rExpression->setText( QString::fromStdString(m->getParameterAsString("rExpression")) );
    ui->lineEdit_maxExpression->setText( QString::fromStdString(m->getParameterAsString("maxExpression")) );
    ui->lineEdit_resultName->setText( QString::fromStdString(m->getParameterAsString("resultName")) );
    ui->checkBox_Points->setChecked(QString::fromStdString(m->getParameterAsString("Points")).toInt());
    ui->checkBox_Edges->setChecked(QString::fromStdString(m->getParameterAsString("Edges")).toInt());


    QStringList optionList;
    optionList << "Replace" << "KeepValue" << "KeepLowerValue" << "KeepHigherValue" << "Add";
    foreach(QString s, optionList)
        ui->comboBox_option->addItem(s);


    if (!QString::fromStdString(m->getParameterAsString("PlacementOption")).isEmpty())
        ui->comboBox_option->setCurrentIndex(optionList.indexOf(QString::fromStdString(m->getParameterAsString("PlacementOption"))));

    QObject::connect(ui->pushButton_addInputRasterData, SIGNAL(clicked()), this, SLOT(addRasterData()));
    QObject::connect(ui->pushButton_RExpression, SIGNAL(clicked()), this, SLOT(addR()));
    QObject::connect(ui->pushButton_rExpression, SIGNAL(clicked()), this, SLOT(addr()));
    QObject::connect(ui->pushButton_maxExpression, SIGNAL(clicked()), this, SLOT(addMax()));

}
void GUIMarker::addR() {
    GUIEquationEditor * dlg = new GUIEquationEditor(this->FormulaR, QStringList(), this);
    QObject::connect(dlg, SIGNAL(values(QStringList)), this, SLOT(addRtoVIBe(QStringList)));
    dlg->addString(ui->lineEdit_RExpression->text());
    dlg->show();
}
void GUIMarker::addRtoVIBe(QStringList list) {
    m->setParameterValue("RExpression", list[1].toStdString());
    ui->lineEdit_RExpression->setText(list[1]);
}
void GUIMarker::addr() {
    GUIEquationEditor * dlg = new GUIEquationEditor(this->Formular, QStringList(), this);
    QObject::connect(dlg, SIGNAL(values(QStringList)), this, SLOT(addrtoVIBe(QStringList)));
    dlg->addString(ui->lineEdit_rExpression->text());
    dlg->show();
}
void GUIMarker::addrtoVIBe(QStringList list) {
    m->setParameterValue("rExpression", list[1].toStdString());
    ui->lineEdit_rExpression->setText(list[1]);
}
void GUIMarker::addMax() {
    GUIEquationEditor * dlg = new GUIEquationEditor(this->FormulaMax, QStringList(), this);
    QObject::connect(dlg, SIGNAL(values(QStringList)), this, SLOT(addMaxtoVIBe(QStringList)));
    dlg->show();
}
void GUIMarker::addMaxtoVIBe(QStringList list) {
    m->setParameterValue("maxExpression", list[1].toStdString());
    ui->lineEdit_maxExpression->setText(list[1]);
}
void GUIMarker::addRasterData() {
    bool ok;
    QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                         tr("Name:"), QLineEdit::Normal, "" , &ok);
    if (ok && !text.isEmpty()) {
        //m->appendToUserDefinedParameter("InputRasterData", text.toStdString());
        ui->listWidget_InputRasterData->addItem(text);
    }


}
GUIMarker::~GUIMarker()
{
    delete ui;
}
void GUIMarker::accept() {
    this->m->setParameterValue("Height", ui->lineEdit_Height->text().toStdString());
    this->m->setParameterValue("Width", ui->lineEdit_Width->text().toStdString());
    this->m->setParameterValue("CellSize", ui->lineEdit_CellSize->text().toStdString());
    this->m->setParameterValue("OffsetX", ui->lineEdit_OffsetX->text().toStdString());
    this->m->setParameterValue("OffsetY", ui->lineEdit_OffsetY->text().toStdString());

    this->m->setParameterValue("resultName", ui->lineEdit_resultName->text().toStdString());
    this->m->setParameterValue("Identifier", ui->comboBox->currentText().toStdString());
    this->m->setParameterValue("rExpression", ui->lineEdit_rExpression->text().toStdString());
    this->m->setParameterValue("RExpression", ui->lineEdit_RExpression->text().toStdString());
    this->m->setParameterValue("maxExpression", ui->lineEdit_maxExpression->text().toStdString());
    this->m->setParameterValue("PlacementOption", ui->comboBox_option->currentText().toStdString());
    this->m->setParameterValue("Edges", QString::number(ui->checkBox_Edges->isChecked()).toStdString());
    this->m->setParameterValue("Points", QString::number(ui->checkBox_Points->isChecked()).toStdString());


    QDialog::accept();
}
