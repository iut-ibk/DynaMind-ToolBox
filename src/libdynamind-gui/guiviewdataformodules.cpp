/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
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

#include "guiviewdataformodules.h"
#include "ui_guiviewdataformodules.h"
#include <dmmodule.h>
#include <dm.h>

QTreeWidgetItem* CreateAttributeItem(QString access, std::string name, const DM::View& v)
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
	return item_attribute;
}



typedef std::map<std::string, std::map<std::string,DM::View> > view_map;

GUIViewDataForModules::GUIViewDataForModules(DM::Module * m, QWidget *parent) :
	QDialog(parent), ui(new Ui::GUIViewDataForModules)
{
	ui->setupUi(this);
	this->m = m;

	//DM::Logger(DM::Debug) << this->m->getName();

	this->ui->treeWidget_views->setColumnCount(3);
	this->ui->treeWidget_views->setColumnWidth(0,200);
	QTreeWidgetItem * headerItem = this->ui->treeWidget_views->headerItem();
	headerItem->setText(0, "Data Stream / View");
	headerItem->setText(1, "Type");
	headerItem->setText(2, "Access");

	view_map views = m->getAccessedViews();
	for (view_map::const_iterator it = views.begin(); it != views.end(); ++it) 
	{
		QTreeWidgetItem * root_port = new QTreeWidgetItem();
		this->ui->treeWidget_views->addTopLevelItem(root_port);
		root_port->setText(0, QString::fromStdString(it->first));

		mforeach (const DM::View& v, it->second) 
		{
			//if (v.getName().compare("dummy") == 0)
			//	continue;
			QTreeWidgetItem * item_view = new QTreeWidgetItem();
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

			foreach(std::string s, v.getAllAttributes()) 
			{
				QString typeString;
				switch(v.getAttributeAccessType(s))
				{
				case DM::READ:	typeString = "read";	break;
				case DM::MODIFY:typeString = "modify";	break;
				case DM::WRITE:	typeString = "write";	break;
				}
				item_view->addChild(CreateAttributeItem(typeString,s, v));
			}

			this->ui->treeWidget_views->expandItem(root_port);
		}
	}

	this->ui->treeWidget->setColumnCount(3);
	this->ui->treeWidget->setColumnWidth(0,200);
	headerItem = this->ui->treeWidget->headerItem();
	headerItem->setText(0, "Data Stream / View");
	headerItem->setText(1, "Type");
	headerItem->setText(2, "Path");

	view_map viewsInStream = m->getViewsInStream();
	for (view_map::const_iterator it = viewsInStream.begin();
		it != viewsInStream.end(); ++it) 
		//foreach (DM::Port * p, this->m->getOutPorts())
	{
		//std::string dataname = p->getLinkedDataName();
		//DM::Logger(DM::Debug) << dataname;
		//DM::System * data = this->m->getData(dataname);
		//if (!sys == 0)
		//	continue;

		QTreeWidgetItem * root_port = new QTreeWidgetItem();

		root_port->setText(0, QString::fromStdString(it->first));
		this->ui->treeWidget->addTopLevelItem(root_port);

		mforeach (const DM::View& v, it->second)
		{
			//if (name.compare("dummy") == 0)
			//	continue;
			//DM::Logger(DM::Debug) << name;
			//DM::View * view = sys->getViewDefinition(name);
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

			//DM::Component * c = sys->getComponent(view->getIdOfDummyComponent());
			//if (c == 0) {
			//	continue;
			//}
			//std::map<std::string,std::string> attributes = v.getAllAttributes();
			//for (std::map<std::string,DM::Attribute*>::const_iterator it  = attributes.begin(); it != attributes.end(); ++it) {
			foreach(std::string attributeName, v.getAllAttributes())
			{
				//DM::Logger(DM::Debug) << it->first;
				/*QTreeWidgetItem * item_attribute = new QTreeWidgetItem();
				item_attribute->setText(0, QString::fromStdString(attributeName));
				item_view->addChild(item_attribute);*/
				item_view->addChild(CreateAttributeItem("write",attributeName, v));
			}
		}
		this->ui->treeWidget->expandItem(root_port);
	}
}

GUIViewDataForModules::~GUIViewDataForModules()
{
	delete ui;
}
