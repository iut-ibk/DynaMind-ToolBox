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
#include <dmsimulation.h>
#include <QLayout>
#include <QLabel>
#include <dmmodule.h>
#include <QScrollArea>
#include <dmlogger.h>

#define PARAM_TAB 1
#define PARAM_CONFIG_TAB 2

#define PARAM_LABEL_COLUMN 0
#define PARAM_VALUE_COLUMN 1

GUIContainerGroup::GUIContainerGroup(ContainerGroup * m, QWidget *parent):
	QDialog(parent),
	ui(new Ui::GUIContainerGroup)
{
	ui->setupUi(this);
	ui->configParamArea->setAlignment(Qt::AlignTop);
	
	// hide config tab
	configTabName = ui->tabWidget->tabText(PARAM_CONFIG_TAB);
	configParamWidget = ui->tabWidget->widget(PARAM_CONFIG_TAB);
	on_enableConfig_stateChanged(0);

	this->m = m;

	foreach (std::string s, m->inStreams)
		insertStreamEntry(s, false);

	foreach (std::string s, m->outStreams)
		insertStreamEntry(s, true);

	foreach(DM::Module* child, m->sim->getModules())
		if(child->getOwner() == m)
			childs.push_back(child);

	// init selection and values
	for(std::map<std::string, std::string>::iterator it = m->parameterConfig.begin(); 
		it != m->parameterConfig.end(); ++it)
	{
		ui->selectedParameters->addItem(new QListWidgetItem(QString::fromStdString(it->first)));
	}

	foreach(DM::Module* child, childs)
	{
		const QString modName = QString::fromStdString(child->getName());
		foreach(DM::Module::Parameter* p,child->getParameters())
		{
			QString fullParamName = modName + "::" + QString::fromStdString(p->name);

			if(!map_contains(&m->parameterConfig, fullParamName.toStdString()))
				ui->availableParameters->addItem(fullParamName);
		}
		if(!map_contains(&childModules, child->getName()))
			childModules[child->getName()] = child;
		else
			DM::Logger(Warning) << "group module names not unique";
	}
}

GUIContainerGroup::~GUIContainerGroup()
{
	delete ui;
}

std::string GUIContainerGroup::getParamValue(const std::string& originalParamString)
{
	QStringList sl = QString::fromStdString(originalParamString).split("::",QString::SkipEmptyParts);
	if(sl.size() != 2)
	{
		DM::Logger(Error) << "could not get parameter value";
		return "";
	}

	if(DM::Module* m = childModules[sl[0].toStdString()])
		return m->getParameterAsString(sl[1].toStdString());
	else
	{
		DM::Logger(Error) << "module '" << sl[0].toStdString() << "' not found";
		return "";
	}
}

void GUIContainerGroup::setParamValue(const std::string& paramName, const std::string& value)
{
	QString originalParamString = "";
	for(std::map<std::string, std::string>::iterator it = m->parameterConfig.begin();
		it != m->parameterConfig.end(); ++it)
		if(it->second == paramName)
			originalParamString = QString::fromStdString(it->first);

	if(originalParamString.length() == 0)
	{
		DM::Logger(Error) << "parameter not found";
		return;
	}
	QStringList sl = originalParamString.split("::",QString::SkipEmptyParts);
	if(sl.size() != 2)
	{
		DM::Logger(Error) << "could not get parameter value";
		return;
	}

	if(DM::Module* m = childModules[sl[0].toStdString()])
		m->setParameterValue(sl[1].toStdString(), value);
	else
		DM::Logger(Error) << "module not found";
}

void GUIContainerGroup::on_addParameter_clicked()
{
	foreach(QListWidgetItem* it, ui->availableParameters->selectedItems())
	{
		m->parameterConfig[it->text().toStdString()] = it->text().toStdString();
		ui->selectedParameters->addItem(it->clone());
		delete it;
	}
}

void GUIContainerGroup::on_rmParameter_clicked()
{
	foreach(QListWidgetItem* it, ui->selectedParameters->selectedItems())
	{
		m->parameterConfig.erase(it->text().toStdString());

		ui->availableParameters->addItem(it->clone());
		delete it;
	}
}

void GUIContainerGroup::on_selectedParameters_itemSelectionChanged()
{
	ui->editParamName->setEnabled(false);
	foreach(QListWidgetItem* it, ui->selectedParameters->selectedItems())
	{
		ui->editParamName->setText(QString::fromStdString(m->parameterConfig[it->text().toStdString()]));
		ui->editParamName->setEnabled(true);
	}
}

void GUIContainerGroup::on_editParamName_textEdited(const QString& newText)
{
	foreach(QListWidgetItem* it, ui->selectedParameters->selectedItems())
		m->parameterConfig[it->text().toStdString()] = newText.toStdString();
}

void GUIContainerGroup::on_tabWidget_currentChanged()
{
	if(ui->tabWidget->currentIndex() == PARAM_TAB)
	{
		for(int i=1; i<ui->configParamArea->rowCount(); i++)
		{
			if(QLayoutItem* it = ui->configParamArea->itemAtPosition(i, PARAM_LABEL_COLUMN))
				delete it->widget();
			if(QLayoutItem* it = ui->configParamArea->itemAtPosition(i, PARAM_VALUE_COLUMN))
				delete it->widget();
		}

		// reconstruct all elements
		int i=1;
		//mforeach(std::string s, parameterRenameMap)
		for(std::map<std::string, std::string>::iterator it = m->parameterConfig.begin(); 
			it != m->parameterConfig.end(); ++it)
		{
			// map value = new name
			ui->configParamArea->addWidget(new QLabel(QString::fromStdString(it->second)), i, PARAM_LABEL_COLUMN);
			QLineEdit* edit = new QLineEdit(QString::fromStdString(getParamValue(it->first)));
			// map key = module::param_name
			ui->configParamArea->addWidget(edit, i, PARAM_VALUE_COLUMN);
			i++;
		}
	}
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

void GUIContainerGroup::on_enableConfig_stateChanged(int state)
{
	if(state > 0)
		ui->tabWidget->addTab(configParamWidget, configTabName);
	else
		ui->tabWidget->removeTab(PARAM_CONFIG_TAB);
}

void GUIContainerGroup::accept() 
{
	for(int i=1; i<ui->configParamArea->rowCount(); i++)
	{
		QLabel* label = NULL;
		QLineEdit* value = NULL;

		if(QLayoutItem* it = ui->configParamArea->itemAtPosition(i, PARAM_LABEL_COLUMN))
			label = dynamic_cast<QLabel*>(it->widget());
		if(QLayoutItem* it = ui->configParamArea->itemAtPosition(i, PARAM_VALUE_COLUMN))
			value = dynamic_cast<QLineEdit*>(it->widget());

		if(label && value)
			setParamValue(label->text().toStdString(), value->text().toStdString());
	}

	QDialog::accept();
}
