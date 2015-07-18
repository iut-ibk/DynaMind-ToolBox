/**
 * @file
 * @author  Gregor Burger <burger.gregor@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2012  Gregor Burger
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "dmaddlayerdialog.h"
#include "ui_dmaddlayerdialog.h"
#include <QColorDialog>
#include <QLabel>
#include <QTreeWidgetItem>
#include <QGroupBox>

#include <sstream>
#include "dmattribute.h"
#include "dmlayer.h"
#include "dmviewer.h"
#include "dmlogger.h"
#include "dmcolorramp.h"

namespace DM {

AddLayerDialog::AddLayerDialog(const std::map<std::string, DM::View>& views, QWidget *parent) :
	QDialog(parent), views(views),view(NULL),
	ui(new Ui::AddLayerDialog) 
{
	ui->setupUi(this);
	start = Qt::white;
	stop = Qt::black;
	mforeach(const DM::View& v, views) 
	{
		QStringList strings;
		strings << QString::fromStdString(v.getName());

		switch (v.getType()) {
		case DM::COMPONENT:
			strings << "Component";
			break;
		case DM::FACE:
			strings << "Face";
			break;
		case DM::EDGE:
			strings << "Edge";
			break;
		case DM::NODE:
			strings << "Node";
			break;
		case DM::RASTERDATA:
			strings << "RasterData";
			break;
		default:
			continue;
		}

		QTreeWidgetItem *item = new QTreeWidgetItem(strings);
		ui->viewList->addTopLevelItem(item);
	}
	QStringList color_ramp_names;
	for (int i = 0; i < LAST_COLOR; i++) {
		color_ramp_names << ColorRampNames[i];
	}
	ui->colorRamp->addItems(color_ramp_names);
}

AddLayerDialog::~AddLayerDialog() {
	delete ui;
}

DM::Layer *AddLayerDialog::getLayer(DM::Viewer *v, System* sys) {
	if (!view)
		return NULL;
	//Change
	//DM::Layer *l = new DM::Layer(system, *view, attribute, ui->checkBox3DObject->isChecked());
	DM::Layer *l = new DM::Layer(sys, *view, attribute, ui->checkBox3DObject->isChecked(), ui->checkBoxAsMesh->isChecked(), ui->checkBoxAsLine->isChecked());
	if (ui->colorCheckBox->isChecked()) {
		v->makeCurrent();
		l->setColorInterpretation(get_color_ramp((ColorRamp)ui->colorRamp->currentIndex(),  l->LayerColor));
	}
	if (ui->heightCheckBox->isChecked()) {
		l->setHeightInterpretation(ui->heightSpinBox->value());
	}
	//CHANGE
	//l->setColorInterpretation(get_color_ramp((ColorRamp)(1), l->LayerColor));

	return l;
}

QStringList AddLayerDialog::getAttributeVectorNames() const {
	if (attribute == "")
		return QStringList();
	// TODO
	/*
	std::vector<Component*> comps = system->getAllComponentsInView(*view);
	if (!comps.size())
		return QStringList();

	Attribute *attr = comps[0]->getAttribute(attribute);
	if (attr->getType() == Attribute::DOUBLEVECTOR) {
		int len = attr->getDoubleVector().size();
		QStringList list;
		for (int i = 0; i < len; i++) {
			list << QString("%1").arg(i);
		}
	}
	if (attr->getType() == Attribute::TIMESERIES) {
		QStringList list;
		foreach(std::string s, attr->getStringVector()) {
			list << QString::fromStdString(s);
		}
		return list;
	}*/
	return QStringList();
}

bool AddLayerDialog::isOverdrawLayer() const {
	return ui->overdraw->isChecked();
}

void AddLayerDialog::on_viewList_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *) {
	ui->attributeList->clear();
	ui->overdraw->setEnabled(current);

	if(!map_contains(&views, current->text(0).toStdString()))
	{
		DM::Logger(Error) << "view " << current->text(0).toStdString() << "not found";
		return;
	}
	else
		view = &views.find(current->text(0).toStdString())->second;

	if (view->getType() == DM::RASTERDATA)
		ui->interpreteGroup->setEnabled(current);
	else
		ui->interpreteGroup->setDisabled(current);

	foreach(std::string name, view->getAllAttributes())
	{
		Attribute::AttributeType type = view->getAttributeType(name);
		QStringList strings;
		strings << QString::fromStdString(name);
		strings << Attribute::getTypeName(type);
		QTreeWidgetItem *item = new QTreeWidgetItem(strings);
		ui->attributeList->addTopLevelItem(item);

		if(	type != Attribute::DOUBLE && 
			type != Attribute::DOUBLEVECTOR && 
			type != Attribute::TIMESERIES)
		{
			item->setDisabled(true);
		}
	}
}

void AddLayerDialog::on_attributeList_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous) {
	attribute = current ? current->text(0).toStdString() : "";
	ui->interpreteGroup->setEnabled(current);
}

string AddLayerDialog::newAttributeName(string viewName, string name)
{
	if (viewName.empty())
		return name;
	std::stringstream ss;
	ss << viewName << ":" <<name;
	return ss.str();
}

}
