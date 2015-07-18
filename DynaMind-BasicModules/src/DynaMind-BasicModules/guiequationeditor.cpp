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
#include "guiequationeditor.h"
#include "ui_guiequationeditor.h"

GUIEquationEditor::GUIEquationEditor(QStringList SingelValues, QStringList Vectors ,QWidget *parent) :
		QDialog(parent),
		ui(new Ui::GUIEquationEditor)
{
	ui->setupUi(this);

	ui->listWidget_Values->addItems(SingelValues);
	ui->listWidget_Vectors->addItems(Vectors);

	QObject::connect(ui->pushButton_plus, SIGNAL(clicked()), this, SLOT(addPlus()));
	QObject::connect(ui->pushButton_minus, SIGNAL(clicked()), this, SLOT(addMinus()));
	QObject::connect(ui->pushButton_multi, SIGNAL(clicked()), this, SLOT(addMulti()));
	QObject::connect(ui->pushButton_cos, SIGNAL(clicked()), this, SLOT(addCos()));
	QObject::connect(ui->pushButton_sin, SIGNAL(clicked()), this, SLOT(addSin()));
	QObject::connect(ui->pushButton_tan, SIGNAL(clicked()), this, SLOT(addTan()));
	QObject::connect(ui->pushButton_nov, SIGNAL(clicked()), this, SLOT(addNov()));
	QObject::connect(ui->pushButton_random, SIGNAL(clicked()), this, SLOT(addRandom()));
	QObject::connect(ui->pushButton_if, SIGNAL(clicked()), this, SLOT(addIf()));
	QObject::connect(ui->pushButton_values, SIGNAL(clicked()), this, SLOT(addValue()));
	QObject::connect(ui->pushButton_vectors, SIGNAL(clicked()), this, SLOT(addVector()));


}
void GUIEquationEditor::hideName() {
	this->ui->label_name->setVisible(false);
	this->ui->lineEdit_name->setVisible(false);
}

void GUIEquationEditor::addVector()
{
	this->addString(ui->listWidget_Vectors->currentItem()->text());
}
void GUIEquationEditor::addValue()
{
	this->addString(ui->listWidget_Values->currentItem()->text());
}

GUIEquationEditor::~GUIEquationEditor()
{
	delete ui;
}

void GUIEquationEditor::addString(QString string) {
	int pos = ui->lineEdit_exp->cursorPosition();
	QString text = ui->lineEdit_exp->text();
	text.insert(pos, string);
	ui->lineEdit_exp->setText(text);
	ui->lineEdit_exp->setCursorPosition(pos+string.length());
	ui->lineEdit_exp->setFocus();
}

void GUIEquationEditor::accept() {

	QStringList list;
	list << ui->lineEdit_name->text() << ui->lineEdit_exp->text();
	emit values(list);
	QDialog::accept();
}
void GUIEquationEditor::setFormula(QString name) {
	this->ui->lineEdit_exp->setText(name);
}

void GUIEquationEditor::setName(QString name) {
	this->ui->lineEdit_name->setText(name);
}
