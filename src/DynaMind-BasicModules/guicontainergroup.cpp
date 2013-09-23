/**
* @file
* @author  Markus Sengthaler <m.sengthaler@gmail.com>
* @version 1.0
* @section LICENSE
*
* This file is part of DynaMind
*
* Copyright (C) 2013  Markus Sengthaler

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

#include "guicontainergroup.h"
#include "ui_guicontainergroup.h"
#include <containergroup.h>
#include <QInputDialog>
#include <QMessageBox>

GUIContainerGroup::GUIContainerGroup(ContainerGroup * m, QWidget *parent):
	QDialog(parent),
	ui(new Ui::GUIContainerGroup)
{
	ui->setupUi(this);
	this->m = m;

	foreach (std::string s, m->inStreams)
		insertStreamEntry(s, false);

	foreach (std::string s, m->outStreams)
		insertStreamEntry(s, true);
}

GUIContainerGroup::~GUIContainerGroup()
{
	delete ui;
}

void GUIContainerGroup::insertStreamEntry(std::string name, bool out)
{
	if(out)
		ui->streamList->addItem(QString::fromStdString(name) + "\tout");
	else
		ui->streamList->addItem(QString::fromStdString(name) + "\tin");
}

void GUIContainerGroup::on_addInStream_clicked()
{
	QString name = QInputDialog::getText(this, "set name", "specify name of read stream");
	if(!name.isEmpty())
	{
		if(m->addStream(name.toStdString(), false))
			insertStreamEntry(name.toStdString(), false);
		else
		{
			QMessageBox box;
			box.setText("Name already taken");
			box.exec();
		}	
	}
}
void GUIContainerGroup::on_addOutStream_clicked()
{
	QString name = QInputDialog::getText(this, "set name", "specify name of write stream");
	if(!name.isEmpty())
	{
		if(m->addStream(name.toStdString(), true))
			insertStreamEntry(name.toStdString(), true);
		else	
		{
			QMessageBox box;
			box.setText("Name already taken");
			box.exec();
		}	
	}
}
void GUIContainerGroup::on_removeStream_clicked()
{
	QListWidgetItem* item = ui->streamList->currentItem();
	if(!item)
		return;
	
	QString qname = item->text();
	if(qname.endsWith("\tin"))
		qname.chop(3);
	else if(qname.endsWith("\tout"))
		qname.chop(4);

	std::string name = qname.toStdString(); 

	if(m->removeStream(name))
		delete ui->streamList->currentItem();
	else
	{
		QMessageBox box;
		box.setText("stream '" + item->text() + "' not found");
		box.exec();
	}
}

void GUIContainerGroup::accept() 
{
	QDialog::accept();
}
