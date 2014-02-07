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

#include <QListWidget>

void DisableItem(QListWidget* content, int item)
{
	if (QListWidgetItem* it = content->item(item))
	{
		it->setFlags(Qt::ItemIsEnabled);
		it->setTextColor(Qt::gray);
	}
}

GUIAttributeCalculator::GUIAttributeCalculator(DM::Module * m, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GUIAttributeCalculator)
{
	ui->setupUi(this);
	this->attrcalc = (AttributeCalculator*) m;

	this->ui->asVector->setChecked(attrcalc->asVector);

	QStringList headers;
	headers << "Name" << "Landscape";
	ui->varaibleTable->setHorizontalHeaderLabels(headers);
	ui->varaibleTable->setColumnWidth (0,320);
	ui->varaibleTable->setColumnWidth (1,320);
	ui->comboView->clear();

	ui->lineExpression->setText(QString::fromStdString( this->attrcalc->getParameterAsString("equation")));
	QString newAttrName = QString::fromStdString(this->attrcalc->getParameterAsString("nameOfNewAttribute"));
	ui->lineEditAttribute->setText(newAttrName);
	int type = (*(int*)this->attrcalc->getParameter("typeOfNewAttribute")->data);
	ui->attributeType->setCurrentIndex(type);

	// a workaround to enable disabled items in combo boxes
	QListWidget* content = new QListWidget(ui->attributeType);
	ui->attributeType->setModel(content->model());
	content->hide();

	content->addItem("no type");
	content->addItem("double");
	content->addItem("string");
	content->addItem("time series");
	content->addItem("link");
	content->addItem("double vector");
	content->addItem("string vector");

	DisableItem(content, 0);
	DisableItem(content, 3);
	DisableItem(content, 4);
	DisableItem(content, 5);
	DisableItem(content, 6);
	// disabled items do not change current index
	ui->attributeType->setCurrentIndex(1);
	// finished workaround

	std::map<std::string, DM::View> views = attrcalc->getViewsInStdStream();

	mforeach(DM::View v, views)
		ui->comboView->addItem(QString::fromStdString(v.getName()));

	if (views.size() > 0)
	{
		std::string n = this->attrcalc->getParameterAsString("NameOfBaseView");
		if (n.empty())
			viewName = QString::fromStdString(views.begin()->second.getName());
		else
		{
			int index = ui->comboView->findText(QString::fromStdString(n));
			if (index != -1) {
				ui->comboView->setCurrentIndex(index);
			}
		}
		this->updateAttributeView();
	}

	// needs viewName
	on_lineEditAttribute_textChanged(newAttrName);

	//CreateVaraibles List
	for (std::map<std::string, std::string>::iterator it = attrcalc->variablesMap.begin(); it != attrcalc->variablesMap.end(); ++it)
	{
		ui->varaibleTable->setRowCount( ui->varaibleTable->rowCount()+1);
		QTableWidgetItem * item = new QTableWidgetItem(QString::fromStdString(it->first));
		ui->varaibleTable->setItem(ui->varaibleTable->rowCount()-1,0, item);
		item = new QTableWidgetItem(QString::fromStdString(it->second));
		ui->varaibleTable->setItem(ui->varaibleTable->rowCount()-1,1, item);
	}
}

void GUIAttributeCalculator::on_lineEditAttribute_textChanged(QString attrName)
{
	std::map<std::string, DM::View> views = attrcalc->getViewsInStdStream();
	if (map_contains(&views, viewName.toStdString()))
	{
		const DM::View& view = views[viewName.toStdString()];
		if (view.hasAttribute(attrName.toStdString()))
		{
			ui->attributeType->setDisabled(true);
			ui->attributeType->setCurrentIndex(view.getAttributeType(attrName.toStdString()));
		}
		else
			ui->attributeType->setDisabled(false);
	}

}


GUIAttributeCalculator::~GUIAttributeCalculator()
{
	delete ui;
}

void GUIAttributeCalculator::createTreeViewEntries(QTreeWidgetItem * root_port, std::string viewname)
{

   // std::vector<std::string> views= this->attrcalc->getSystemIn()->getNamesOfViews();
	//foreach (std::string vn, views) {
	//    DM::View * v = this->attrcalc->getSystemIn()->getViewDefinition(vn);

	mforeach(DM::View v, attrcalc->getViewsInStdStream())
	{
		if (v.getName().compare("dummy") == 0)
			continue;
		if (v.getName().compare(viewname) != 0)
			continue;

		//DM::Component * c = this->attrcalc->getSystemIn()->getComponent(v->getIdOfDummyComponent());
		if (v.getAllAttributes().size() == 0) {
			continue;
		}
		//Check if View has already a parent with the same name
		QTreeWidgetItem * parent = root_port->parent();
		while (parent) {
			std::string sparent = parent->text(0).toStdString();
			if (sparent.compare(viewname) == 0)
				return;
			parent  = parent->parent();
		}
		foreach(std::string s, v.getAllAttributes())
		{
		//std::map<std::string,DM::Attribute*> attributes = c->getAllAttributes();
		//for (std::map<std::string,DM::Attribute*>::const_iterator it  = attributes.begin(); it != attributes.end(); ++it) {
		//    DM::Logger(DM::Debug) << it->first;
		//    Attribute * attr = it->second;
			QTreeWidgetItem * item_attribute = new QTreeWidgetItem();

			item_attribute->setText(0, QString::fromStdString(s));
			root_port->addChild(item_attribute);
			if (v.getAttributeType(s) == Attribute::LINK) {
				createTreeViewEntries(item_attribute, v.getNameOfLinkedView(s));
			}
		}


	}
}

void GUIAttributeCalculator::updateAttributeView()
{
	//std::vector<std::string> views= this->attrcalc->getSystemIn()->getNamesOfViews();

	ui->listAttributes->clear();
	if (viewName.isEmpty())
		return;
	mforeach(DM::View v, attrcalc->getViewsInStdStream())
	{
		std::string strView = v.getName();
		if (strView.compare("dummy") == 0)
			continue;
		if (strView.compare(viewName.toStdString()) != 0)
			continue;
		QTreeWidgetItem * headerItem = this->ui->listAttributes->headerItem();
		headerItem->setText(0, "View");
		headerItem->setText(1, "Attribute");
		QTreeWidgetItem * root_port = new QTreeWidgetItem();
		this->ui->listAttributes->addTopLevelItem(root_port);
		root_port->setText(0, QString::fromStdString(strView));
		this->createTreeViewEntries(root_port, strView);
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

	if(!ui->listAttributes->currentItem())
	{
		DM::Logger(Error) << "please select an attribute";
		return;
	}
	//elementName << ui->comboView->currentText().toStdString();

	std::vector<std::string> elements;
	elements.push_back(ui->listAttributes->currentItem()->text(0).toStdString());
	QTreeWidgetItem * p = ui->listAttributes->currentItem()->parent();

	do {
		elements.push_back(p->text(0).toStdString());
		p = p->parent();
	} while (p);

	elementName << elements[elements.size()-1];
	for (int i = elements.size()-2; i > -1; --i)
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
	this->updateAttributeView();

}

void GUIAttributeCalculator::accept() {
	this->attrcalc->setParameterValue("NameOfBaseView", viewName.toStdString());
	this->attrcalc->setParameterValue("equation", ui->lineExpression->text().toStdString());
	this->attrcalc->setParameterValue("nameOfNewAttribute", ui->lineEditAttribute->text().toStdString());
	*(int*)this->attrcalc->getParameter("typeOfNewAttribute")->data = ui->attributeType->currentIndex();

	int rows = ui->varaibleTable->rowCount();

	//std::map<std::string, std::string> variables;
	for (int i = 0; i < rows; i++) {
		this->attrcalc->variablesMap[ui->varaibleTable->item(i, 0)->text().toStdString()] = ui->varaibleTable->item(i, 1)->text().toStdString();

	}
	//this->attrcalc->setParameterNative<std::map<std::string, std::string > >("variablesMap", variables);
	//this->attrcalc->setParameterNative<bool>("asVector", this->ui->asVector->isChecked());
	this->attrcalc->asVector = this->ui->asVector->isChecked();
	this->attrcalc->init();
	QDialog::accept();
}
