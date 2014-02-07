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

void DisableTypeItem(QListWidget* content, int item)
{
	if (QListWidgetItem* it = content->item(item))
	{
		it->setFlags(Qt::ItemIsEnabled);
		it->setTextColor(Qt::gray);
	}
}

bool IsTypeDisabled(QListWidget* content, int item)
{
	if (QListWidgetItem* it = content->item(item))
		return it->flags() | Qt::ItemIsEnabled;
	return false;
}

GUIAttributeCalculator::GUIAttributeCalculator(DM::Module * m, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GUIAttributeCalculator)
{
	ui->setupUi(this);
	this->attrcalc = (AttributeCalculator*) m;

	QStringList headers;
	headers << "Name" << "Landscape";
	ui->variableTable->setHorizontalHeaderLabels(headers);
	ui->variableTable->horizontalHeader()->setStretchLastSection(true);
	ui->comboView->clear();

	ui->lineExpression->setText(QString::fromStdString( this->attrcalc->getParameterAsString("equation")));
	QString newAttrName = QString::fromStdString(this->attrcalc->getParameterAsString("nameOfNewAttribute"));
	ui->lineEditAttribute->setText(newAttrName);

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

	DisableTypeItem(content, 0);
	DisableTypeItem(content, 3);
	DisableTypeItem(content, 4);

	// finished workaround
	int type = (*(int*)this->attrcalc->getParameter("typeOfNewAttribute")->data);
	if (IsTypeDisabled(content, type))
		ui->attributeType->setCurrentIndex(type);
	else
		ui->attributeType->setCurrentIndex(1);

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

	// create variables List
	for (std::map<std::string, std::string>::iterator it = attrcalc->variablesMap.begin(); it != attrcalc->variablesMap.end(); ++it)
		addVariableItem(QString::fromStdString(it->first), QString::fromStdString(it->second));

	ui->addButton->setDisabled(true);
}

void GUIAttributeCalculator::addVariableItem(const QString& reference, const QString& variableName)
{
	int numRows = ui->variableTable->rowCount();
	ui->variableTable->setRowCount(numRows + 1);
	QTableWidgetItem * item = new QTableWidgetItem(reference);
	item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	ui->variableTable->setItem(numRows, 0, item);
	item = new QTableWidgetItem(variableName);
	ui->variableTable->setItem(numRows, 1, item);

	ui->variableTable->resizeColumnToContents(0);
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

void GUIAttributeCalculator::on_listAttributes_currentItemChanged(QTreeWidgetItem * current, QTreeWidgetItem * previous)
{
	ui->addButton->setEnabled(current->parent());
}

void GUIAttributeCalculator::on_variableTable_cellChanged(int row, int column)
{
	if (ui->variableTable->item(row, column)->text().isEmpty())
		ui->variableTable->removeRow(ui->variableTable->currentRow());
}

GUIAttributeCalculator::~GUIAttributeCalculator()
{
	delete ui;
}
/*
void GUIAttributeCalculator::createTreeViewEntries(QTreeWidgetItem * root_port, std::string viewname)
{

   // std::vector<std::string> views= this->attrcalc->getSystemIn()->getNamesOfViews();
	//foreach (std::string vn, views) {
	//    DM::View * v = this->attrcalc->getSystemIn()->getViewDefinition(vn);

	if (viewname == "dummy")
		return;

	DM::View v;
	std::map<std::string, View> stream = attrcalc->getViewsInStdStream();

	if (map_contains(&stream, viewname, v))
	{
		//DM::Component * c = this->attrcalc->getSystemIn()->getComponent(v->getIdOfDummyComponent());
		if (v.getAllAttributes().size() == 0)
			return;

		// Check if View has already a parent with the same name
		QTreeWidgetItem * parent = root_port->parent();
		while (parent) 
		{
			if (viewname == parent->text(0).toStdString())
				return;
			parent = parent->parent();
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
}*/


void GUIAttributeCalculator::addViewToTree(const DM::View& v)
{
	if (v.getName().empty() || v.getName() == "dummy")
		return;

	QTreeWidgetItem * viewItem = new QTreeWidgetItem(QStringList(QString::fromStdString(v.getName())));
	ui->listAttributes->addTopLevelItem(viewItem);

	if (v.getAllAttributes().size() == 0)
	{
		viewItem->setDisabled(true);
		return;
	}

	foreach(std::string attrName, v.getAllAttributes())
	{
		QTreeWidgetItem* attrItem = new QTreeWidgetItem(QStringList(QString::fromStdString(attrName)));
		viewItem->addChild(attrItem);

		if (v.getAttributeType(attrName) == DM::Attribute::LINK)
			attrItem->setDisabled(true);
	}
}

void GUIAttributeCalculator::updateAttributeView()
{
	ui->listAttributes->clear();

	mforeach(const DM::View& v, attrcalc->getViewsInStdStream())
		addViewToTree(v);
}


void GUIAttributeCalculator::on_addButton_clicked() 
{
	if (ui->listAttributes->size().isNull())
		return;

	if(!ui->listAttributes->currentItem())
	{
		DM::Logger(Error) << "please select an attribute";
		return;
	}

	bool ok;
	QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
		tr("variable Name"), QLineEdit::Normal,
		"", &ok);
	if (!ok || text.isEmpty())
		return;

	std::vector<std::string> elements;
	elements.push_back(ui->listAttributes->currentItem()->text(0).toStdString());
	QTreeWidgetItem * p = ui->listAttributes->currentItem()->parent();

	do {
		elements.push_back(p->text(0).toStdString());
		p = p->parent();
	} while (p);

	std::stringstream elementName;
	elementName << elements[elements.size()-1];
	for (int i = elements.size()-2; i > -1; --i)
		elementName << "." <<elements[i];

	addVariableItem(QString::fromStdString(elementName.str()), text);
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


	for (int i = 0; i < ui->variableTable->rowCount(); i++) {
		const QTableWidgetItem* rowItem = ui->variableTable->item(i, 0);
		const QTableWidgetItem* columnItem = ui->variableTable->item(i, 1);
		this->attrcalc->variablesMap[rowItem->text().toStdString()] = columnItem->text().toStdString();
	}

	this->attrcalc->init();
	QDialog::accept();
}
