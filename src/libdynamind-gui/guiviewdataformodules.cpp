/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 * This file is part of DynaMind
 *
 * Copyright (C) 2011-2014  Christian Urich, Markus Sengthaler

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

#include "guiviewdataformodules.h"
#include "ui_guiviewdataformodules.h"
#include <dmmodule.h>
#include <dmgroup.h>
#include <dmsimulation.h>
#include <dm.h>

QTreeWidgetItem* CreateAttributeItem(QString access, std::string name, 
	const DM::View& v, const QColor& color = Qt::white)
{
	QTreeWidgetItem * item_attribute = new QTreeWidgetItem();
	item_attribute->setText(0, QString::fromStdString(name));
	switch(v.getAttributeType(name))
	{
	case DM::Attribute::DOUBLE:
		item_attribute->setText(1, "double"); break;
	case DM::Attribute::DOUBLEVECTOR:
		item_attribute->setText(1, "doublevector"); break;
	case DM::Attribute::LINK:
		item_attribute->setText(1, "link"); break;
	case DM::Attribute::NOTYPE:
		item_attribute->setText(1, "no type"); break;
	case DM::Attribute::STRING:
		item_attribute->setText(1, "string"); break;
	case DM::Attribute::STRINGVECTOR:
		item_attribute->setText(1, "stringvector"); break;
	case DM::Attribute::TIMESERIES:
		item_attribute->setText(1, "time series"); break;
	}
	item_attribute->setText(2, access);

	for (int i = 0; i < 3; i++)
		item_attribute->setBackgroundColor(i, color);

	return item_attribute;
}

typedef std::map<std::string, DM::View> view_map;
typedef std::map<std::string, std::map<std::string, DM::View> > stream_map;

void GetAccessedViews(DM::Module* m, const std::string& inPort, view_map& accessed_views)
{
	mforeach(const DM::View& v, m->getAccessedViews()[inPort])
	{
		// geometry
		if (!map_contains(&accessed_views, v.getName()))
			accessed_views[v.getName()] = v;

		// attributes
		foreach(std::string attrName, v.getAllAttributes())
			if (!accessed_views[v.getName()].hasAttribute(attrName))
				accessed_views[v.getName()].addAttribute(attrName, v.getAttributeType(attrName), v.getAttributeAccessType(attrName));
	}
}

void RecursiveGetAccessedViews(DM::Simulation::Link* l, DM::Group* g, view_map& accessed_views)
{
	DM::Module* m = l->dest;
	if (m == g)
		return;

	GetAccessedViews(l->dest, l->inPort, accessed_views);

	foreach(const std::string& outPort, m->getOutPortNames())
		foreach(DM::Simulation::Link* next_link, g->sim->getOutgoingLinks(m, outPort))
			RecursiveGetAccessedViews(next_link, g, accessed_views);
}

void GetAccessedStreamInGroup(DM::Group* g, stream_map& stream)
{
	foreach(std::string inPort, g->getInPortNames())
		foreach(DM::Simulation::Link* link, g->sim->getIntoGroupLinks(g, inPort))
			RecursiveGetAccessedViews(link, g, stream[inPort]);
}

GUIViewDataForModules::GUIViewDataForModules(DM::Module * m, QWidget *parent) :
	QDialog(parent), ui(new Ui::GUIViewDataForModules)
{
	ui->setupUi(this);
	this->m = m;

	this->ui->treeWidget_views->setColumnCount(3);
	this->ui->treeWidget_views->setColumnWidth(0,200);
	QTreeWidgetItem * headerItem = this->ui->treeWidget_views->headerItem();
	headerItem->setText(0, "Data Stream / View");
	headerItem->setText(1, "Type");
	headerItem->setText(2, "Access");

	stream_map accessed_views;

	if (m->isGroup())
		GetAccessedStreamInGroup(dynamic_cast<DM::Group*>(m), accessed_views);
	else
		accessed_views = m->getAccessedViews();


	for (stream_map::const_iterator it = accessed_views.begin(); it != accessed_views.end(); ++it)
	{
		QTreeWidgetItem * root_port = new QTreeWidgetItem();
		this->ui->treeWidget_views->addTopLevelItem(root_port);
		root_port->setText(0, QString::fromStdString(it->first));

		mforeach (const DM::View& v, it->second) 
		{
			QTreeWidgetItem * item_view = new QTreeWidgetItem();

			bool view_missing = false;
			if (v.reads() && !map_contains(&m->getViewsInStream()[it->first], v.getName()))
				view_missing = true;

			item_view->setText(0, QString::fromStdString(v.getName()));
			int type = v.getType();

			switch(v.getType())
			{
			case DM::NODE:			item_view->setText(1, "Nodes");			break;
			case DM::EDGE:			item_view->setText(1, "Edges");			break;
			case DM::FACE:			item_view->setText(1, "Faces");			break;
			case DM::SUBSYSTEM:		item_view->setText(1, "Systems");		break;
			case DM::RASTERDATA:	item_view->setText(1, "Raster Data");	break;
			}

			switch(v.getAccessType())
			{
			case DM::WRITE:		item_view->setText(2, "write");		break;
			case DM::READ:		item_view->setText(2, "read");		break;
			case DM::MODIFY:	item_view->setText(2, "modify");	break;
			}

			root_port->addChild(item_view);

			bool any_attribute_missing = false;
			foreach(std::string s, v.getAllAttributes()) 
			{
				bool attribute_missing = false;
				QString typeString;
				switch(v.getAttributeAccessType(s))
				{
				case DM::READ:
					if (view_missing || !m->getViewsInStream()[it->first][v.getName()].hasAttribute(s))
					{
						any_attribute_missing = true;
						attribute_missing = true;
					}
					typeString = "read";
					break;
				case DM::MODIFY:	typeString = "modify";	break;
				case DM::WRITE:		typeString = "write";	break;
				}
				item_view->addChild(CreateAttributeItem(typeString, s, v, attribute_missing ? Qt::red : Qt::white));
			}

			QColor color = Qt::white;
			if (view_missing)
				color = Qt::red;
			else if (any_attribute_missing)
				color = QColor(255, 160, 0);

			for (int i = 0; i < 3; i++)
				item_view->setBackgroundColor(i, color);

			this->ui->treeWidget_views->expandItem(root_port);
		}
	}

	this->ui->treeWidget->setColumnCount(3);
	this->ui->treeWidget->setColumnWidth(0,200);
	headerItem = this->ui->treeWidget->headerItem();
	headerItem->setText(0, "Data Stream / View");
	headerItem->setText(1, "Type");
	headerItem->setText(2, "Path");

	stream_map viewsInStream = m->getViewsInStream();
	for (stream_map::const_iterator it = viewsInStream.begin(); it != viewsInStream.end(); ++it)
	{
		QTreeWidgetItem * root_port = new QTreeWidgetItem();

		root_port->setText(0, QString::fromStdString(it->first));
		this->ui->treeWidget->addTopLevelItem(root_port);

		mforeach (const DM::View& v, it->second)
		{
			QTreeWidgetItem * item_view = new QTreeWidgetItem();
			item_view->setText(0, QString::fromStdString(v.getName()));

			int type = v.getType();

			if (type == DM::NODE)
				item_view->setText(1, "Nodes");
			if (type == DM::EDGE)
				item_view->setText(1, "Edges");
			if (type == DM::FACE)
				item_view->setText(1, "Faces");
			if (type == DM::SUBSYSTEM)
				item_view->setText(1, "Systems");
			if (type == DM::RASTERDATA)
				item_view->setText(1, "Raster Data");

			root_port->addChild(item_view);

			foreach(std::string attributeName, v.getAllAttributes())
				item_view->addChild(CreateAttributeItem("write",attributeName, v));
		}
		this->ui->treeWidget->expandItem(root_port);
	}
}

GUIViewDataForModules::~GUIViewDataForModules()
{
	delete ui;
}
