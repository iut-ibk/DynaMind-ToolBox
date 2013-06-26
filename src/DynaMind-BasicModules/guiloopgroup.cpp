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


/*

#include "guiloopgroup.h"
#include "ui_guiloopgroup.h"
#include <loopgroup.h>
#include <QInputDialog>

GUILoopGroup::GUILoopGroup(LoopGroup * m, QWidget *parent):
	QDialog(parent),
	ui(new Ui::GUILoopGroup)
{
	ui->setupUi(this);

	this->m = m;

	ui->lineEdit->setText(QString::fromStdString(m->getParameterAsString("Runs")));

	foreach (std::string s, m->nameOfInPorts)
		this->ui->listWidget_in->addItem(QString::fromStdString(s));

	foreach (std::string s, m->nameOfOutPorts)
		this->ui->listWidget_out->addItem(QString::fromStdString(s));
}

GUILoopGroup::~GUILoopGroup()
{
	delete ui;
}

void GUILoopGroup::on_addInPort_clicked() 
{
	bool ok;
	QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
		tr("Name"), QLineEdit::Normal,
		"", &ok);

	if (ok && !text.isEmpty())
	{
		bool isNew = true; 
		foreach(std::string s, m->nameOfInPorts)
			if(s == text.toStdString())
				isNew = false;

		if(isNew)
		{
			this->m->addInPort(text.toStdString());
			this->ui->listWidget_in->addItem(text);
			this->m->nameOfInPorts.push_back(text.toStdString());
		}
	}

}

void GUILoopGroup::on_addOutPort_clicked() 
{
	bool ok;
	QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
		tr("Name"), QLineEdit::Normal,
		"", &ok);

	if (ok && !text.isEmpty()) 
	{
		bool isNew = true; 
		foreach(std::string s, m->nameOfOutPorts)
			if(s == text.toStdString())
				isNew = false;

		if(isNew)
		{
			this->ui->listWidget_out->addItem(text);
			this->m->addOutPort(text.toStdString());
			this->m->nameOfOutPorts.push_back(text.toStdString());
		}
	}

}
void GUILoopGroup::on_rmInport_clicked() 
{
	if (!ui->listWidget_in->currentItem())
		return;
	std::string toRemove = ui->listWidget_in->currentItem()->text().toStdString();
	m->removeInPort(toRemove);

	std::vector<std::string>::iterator it = std::find(m->nameOfInPorts.begin(), m->nameOfInPorts.end(), toRemove);
	if(it != m->nameOfInPorts.end())
		m->nameOfInPorts.erase(it);

	delete this->ui->listWidget_in->currentItem();

}

void GUILoopGroup::on_rmOutport_clicked()
{
	if (!ui->listWidget_out->currentItem())
		return;
	std::string toRemove = ui->listWidget_out->currentItem()->text().toStdString();
	m->removeOutPort(toRemove);

	std::vector<std::string>::iterator it = std::find(m->nameOfOutPorts.begin(), m->nameOfOutPorts.end(), toRemove);
	if(it != m->nameOfOutPorts.end())
		m->nameOfOutPorts.erase(it);

	delete this->ui->listWidget_out->currentItem();
}

void GUILoopGroup::accept() 
{
	m->setParameterValue("Runs", ui->lineEdit->text().toStdString());
	QDialog::accept();
	m->update();
}
*/

