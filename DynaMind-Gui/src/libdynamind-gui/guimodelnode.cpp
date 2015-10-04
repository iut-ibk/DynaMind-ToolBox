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

#include "guimodelnode.h"
#include <iostream>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QString>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QVariant>
#include <QInputDialog>
#include "moduledescription.h"
#include <QGroupBox>
#include <QStringList>
#include <QScrollArea>
#include <QComboBox>
#include <QMap>
#include <QCheckBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QWebView>
#include <QUrl>
#include <dmmodule.h>
#include <dmgroup.h>
#include <groupnode.h>
#include <QTextEdit>
#include <QTableWidget>
#include <QHeaderView>
#include "guihelpviewer.h"
#include <limits>

#include <QDoubleSpinBox>

void GUIModelNode::openFileDialog(QString label)
{
	QString s = QFileDialog::getOpenFileName(this, tr("Open file"), "", tr("Files (*.*)"));
	if (!s.isEmpty())
		((QLineEdit*)elements[label])->setText(s);
}

GUIModelNode::GUIModelNode(DM::Module * m, ModelNode *mn, QWidget* parent) :QWidget(parent)
{
	this->module = m;
	this->modelnode = mn;
	setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);


	QGridLayout *layout = new QGridLayout;
	pathSignalMapper = new QSignalMapper(this);
	connect(pathSignalMapper, SIGNAL(mapped(QString)), this, SLOT(openFileDialog(QString)));

	layout1 = new QGridLayout;
	QGroupBox *gbox = new QGroupBox;
	gbox->setTitle("Parameter");

	foreach(DM::Module::Parameter* p, m->getParameters())
	{
		QString qname  = QString::fromStdString(p->name);

		switch(p->type)
		{
		case DM::DOUBLE:
			{
				layout1->addWidget(new QLabel(qname), layout1->rowCount(), 0);

				QDoubleSpinBox* dsbox = new QDoubleSpinBox();
				dsbox->setMaximum(std::numeric_limits<double>::max());
				dsbox->setMinimum(-std::numeric_limits<double>::max());
				dsbox->setDecimals(5);
				dsbox->setValue(*(double*)p->data);
				elements.insert(qname, dsbox);
				layout1->addWidget(dsbox, layout1->rowCount() - 1, 1);
			}
			break;
		case DM::LONG:
			{
				layout1->addWidget(new QLabel(qname), layout1->rowCount(), 0);

				QDoubleSpinBox* sbox = new QDoubleSpinBox();
				sbox->setMaximum(std::numeric_limits<long>::max());
				sbox->setMinimum(std::numeric_limits<long>::min());
				sbox->setDecimals(0);
				sbox->setSingleStep(1);
				elements.insert(qname, sbox);
				sbox->setValue(*(long*)p->data);
				layout1->addWidget(sbox, layout1->rowCount() - 1, 1);
			}
			break;
		case DM::INT:
			{
				layout1->addWidget(new QLabel(qname), layout1->rowCount(), 0);

				QSpinBox* sbox = new QSpinBox();
				sbox->setMaximum(std::numeric_limits<int>::max());
				sbox->setMinimum(std::numeric_limits<int>::min());
				elements.insert(qname, sbox);
				sbox->setValue(*(int*)p->data);
				layout1->addWidget(sbox, layout1->rowCount() - 1, 1);
			}
			break;
		case DM::BOOL:
			{
				layout1->addWidget(new QLabel(qname), layout1->rowCount(),0);

				QCheckBox * le = new QCheckBox;
				le->setChecked(*(bool*)p->data);
				elements.insert(qname, le);

				layout1->addWidget(le,layout1->rowCount()-1,1);
			}
			break;
		case DM::STRING:
			{
				layout1->addWidget(new QLabel(qname), layout1->rowCount(),0);

				QLineEdit * le = new QLineEdit;
				le->setText(QString::fromStdString(*(std::string*)p->data));
				elements.insert(qname, le);

				layout1->addWidget(le,layout1->rowCount()-1,1);
			}
			break;
		case DM::FILENAME:
			{
				layout1->addWidget(new QLabel(qname), layout1->rowCount(),0);

				QLineEdit * le = new QLineEdit;
				QPushButton * pb = new QPushButton;
				le->setText(QString::fromStdString(*(std::string*)p->data));
				pb->setText("...");
				elements.insert(qname, le);

				layout1->addWidget(le,layout1->rowCount()-1,1);
				layout1->addWidget(pb,layout1->rowCount()-1,2);

				connect(pb, SIGNAL(clicked()), pathSignalMapper, SLOT(map()));
				pathSignalMapper->setMapping(pb, qname);
			}
			break;
		case DM::STRING_LIST:
			{
				layout1->addWidget(new QLabel(qname), layout1->rowCount(),0);

				std::vector<std::string> text = *(std::vector<std::string>*)p->data;
				std::string nlText;
				foreach(std::string str, text)
					nlText += str +'\n';

				QTextEdit *editBox = new QTextEdit();
				editBox->setPlainText(QString::fromStdString(nlText));
				this->elements.insert(QString::fromStdString(p->name), editBox);
				layout1->addWidget(editBox, layout1->rowCount()-1,1);
			}
			break;
		case DM::STRING_MAP:
			{
				QGroupBox* group = new QGroupBox(qname, parent);
				layout1->addWidget(group,layout1->rowCount(),0,1,2);

				QVBoxLayout* verticalLayout = new QVBoxLayout(parent);
				group->setLayout(verticalLayout);

				QTableWidget* table = new QTableWidget(group);
				verticalLayout->addWidget(table);

				table->setColumnCount(2);
				QStringList header;
				header << "key";
				header << "value";
				table->setHorizontalHeaderLabels(header);
				//table->horizontalHeader()->setFixedHeight(20);
				std::map<std::string, std::string>* entries = (std::map<std::string, std::string>*)p->data;

				table->setRowCount(entries->size());
				int i=0;
				for(std::map<std::string, std::string>::const_iterator it = entries->begin(); it != entries->end(); ++it) 
				{
					table->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(it->first)));
					table->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(it->second)));
					//table->setRowHeight(i, 20);
					i++;
				}

				QPushButton* qb = new QPushButton("+", parent);
				qb->setObjectName(qname);
				connect(qb, SIGNAL(clicked()), this, SLOT(addMapRow()));
				verticalLayout->addWidget(qb);

				this->elements.insert(qname, table);

			}
			break;
		}
	}
	gbox->setLayout(layout1);
	layout->addWidget(gbox);

	QDialogButtonBox * bbox  = new QDialogButtonBox(QDialogButtonBox::Ok
		| QDialogButtonBox::Cancel | QDialogButtonBox::Help);

	QScrollArea *bar = new QScrollArea;
	bar->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	bar->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	bar->setWidgetResizable(true);

	QWidget *widget = new QWidget;
	widget->setLayout(layout);
	bar->setWidget(widget);

	QVBoxLayout * l = new QVBoxLayout;
	l->addWidget(bar);
	l->addWidget(bbox);
	setLayout(l);
	connect(bbox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(bbox, SIGNAL(rejected()), this, SLOT(reject()));
	connect(bbox, SIGNAL(helpRequested ()), this, SLOT(help()));
}

void GUIModelNode::addMapRow()
{
	QTableWidget* table = (QTableWidget*)elements[sender()->objectName()];
	int rows = table->rowCount();
	table->setRowCount(rows+1);
	//table->setRowHeight(rows, 20);
}

void GUIModelNode::help() 
{
	/*QWidget * helpWindow = new QWidget(this->parentWidget());
	QTextEdit * text = new QTextEdit();
	QVBoxLayout * vbox = new QVBoxLayout;

	helpWindow->setLayout(vbox);
	helpWindow->layout()->addWidget(text);
	helpWindow->show();*/
	GUIHelpViewer* ghv = new GUIHelpViewer(this->parentWidget());
	ghv->showHelpForModule(modelnode->getModule());
	ghv->show();
}

GUIModelNode::~GUIModelNode() 
{
}

void GUIModelNode::accept() 
{
	foreach(QString s,  this->elements.keys()) 
	{
		DM::Module::Parameter* p = module->getParameter(s.toStdString());
		if(!p)
			continue;
		switch(p->type)
		{
		case (DM::BOOL):
			{
				QAbstractButton *ab = ( QAbstractButton * ) this->elements.value(s);
				*(bool*)p->data = ab->isChecked();
			}
			break;
		case (DM::STRING_LIST):
			{
				QTextEdit* txtEdit = (QTextEdit*) this->elements.value(s);
				QString nlText = txtEdit->toPlainText();
				QStringList strings = nlText.split('\n');
				std::vector<std::string>* parValue = (std::vector<std::string>*)p->data;
				parValue->clear();
				foreach(QString str, strings)
					parValue->push_back(str.toStdString());
			}
			break;
		case (DM::STRING_MAP):
			{
				std::map<std::string, std::string>* map = (std::map<std::string, std::string>*)p->data;
				map->clear();

				QTableWidget* table = (QTableWidget*) this->elements.value(s);
				for(int i=0;i<table->rowCount();i++)
				{
					QTableWidgetItem* itKey = table->item(i,0);
					QTableWidgetItem* itValue = table->item(i,1);
					if(itKey && itValue)
					{
						QString key = itKey->text();
						QString value = table->item(i,1)->text();

						if(key.length() > 0 && value.length() > 0)
							(*map)[table->item(i,0)->text().toStdString()] = table->item(i,1)->text().toStdString();
					}
				}
			}
			break;
		case DM::INT:
			*(int*)p->data = ((QSpinBox*)this->elements.value(s))->value();
			break;
		case DM::LONG:
			*(long*)p->data = ((QSpinBox*)this->elements.value(s))->value();
			break;
		case DM::DOUBLE:
			*(double*)p->data = ((QDoubleSpinBox*)this->elements.value(s))->value();
			break;
		case DM::STRING:
		case DM::FILENAME:
			{
				QLineEdit *le = ( QLineEdit * ) this->elements.value(s);
				*(std::string*)p->data = le->text().toStdString();
			}
			break;
		default:
			break;
		}
	}
	module->init();
	//module->update();	// calls update ports
	if(module->getForceRefreshSimulation())
		emit updateStream();
	delete(this);
}
void GUIModelNode::reject() {
	delete(this);
}
