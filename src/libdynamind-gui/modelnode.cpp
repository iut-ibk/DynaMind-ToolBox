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
#include <modelnode.h>

#include <iostream>
#include <sstream>

#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QInputDialog>

#include "ColorPalette.h"
#include <guimodelnode.h>
#include <guiviewdataformodules.h>
#include <dmmodule.h>
#include <portnode.h>
#include <guisimulation.h>
#include <dmsystem.h>
#include <dm.h>

//#define QGLVIEWER

#ifdef VIEWER
#include <dmviewerwindow.h>
#endif

#include <groupnode.h>

#include <guihelpviewer.h>
#include <qdialog.h>
#include <qtabwidget.h>
#include <simulationtab.h>
#include <dmlogger.h>

#include <guifilter.h>

#include "modelobserver.h"

ModelNode::ModelNode(DM::Module* m, GUISimulation* sim, QGraphicsItem * parent) : QGraphicsItem(parent)
{
	child = NULL;
	module = m;
	ng = NULL;
	width = 50;
	height = 50;

	setAcceptDrops(true);

	simulation = sim;
	this->setFlag(QGraphicsItem::ItemIsSelectable, true);
	this->setFlag(QGraphicsItem::ItemIsMovable, true);
	this->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
	this->setCacheMode(QGraphicsItem::DeviceCoordinateCache);

	resize();

	// be shure to add observer last, as module dimensions are not set before
	new ModelObserver(this, m);

	hovered = false;
	setAcceptHoverEvents(true);

}

ModelNode::~ModelNode() 
{
	QVector<PortNode*>	portCopy = ports;
	ports.clear();

	foreach(PortNode* p, portCopy)
		delete p;
}

void ModelNode::resize()
{
	// module name has to fit in node


	//	// make groups a bit bigger
	//	if(!module->isGroup())
	//		height =  45;
	//	else
	//		height =  65;

	// ports names have to fit
	/*int maxPortSize = 0;
	foreach(PortNode* p, ports)
	maxPortSize = max(maxPortSize, (int)QGraphicsSimpleTextItem(p->getPortName()).boundingRect().width());

	maxPortSize += 20;
	width = max(width, 2*maxPortSize);*/

	// port number has to fit
	//	int maxPortCount = max(module->getInPortNames().size(), module->getOutPortNames().size());
	//	height = max(height, 20 + 15 * maxPortCount);

	// update port pos
	foreach(PortNode* p, ports)
		p->updatePos();

	if(DM::Module* m = module->getOwner())
	{
		if(GroupNode* gn = (GroupNode*)getSimulation()->getModelNode(m)->getChild())
		{
			gn->resize();
		}
	}
}

void ModelNode::setPos(const QPointF &pos)
{
	QGraphicsItem::setPos(pos);

	if(DM::Module* m = module->getOwner())
	{
		if(GroupNode* gn = (GroupNode*)getSimulation()->getModelNode(m)->getChild())
		{
			gn->resize();
		}
	}
}

PortNode* ModelNode::getPort(std::string portName, const DM::PortType type)
{
	foreach(PortNode* p, ports)
		if(p->getPortName().toStdString() == portName && p->getType() == type)
			return p;

	return NULL;
}

QVector<PortNode*> ModelNode::getPorts(DM::PortType type)
{
	QVector<PortNode*> ps;
	foreach(PortNode* p, ports)
		if(p->getType() == type)
			ps.push_back(p);

	return ps;
}

void ModelNode::addPort(const std::string &name, const DM::PortType type)
{
	this->ports.append(new PortNode(QString::fromStdString(name), module, type, this, this->simulation));
	resize();
}
void ModelNode::removePort(const std::string &name, const DM::PortType type)
{
	PortNode* p = getPort(name, type);
	if(p)
		ports.erase(find(ports.begin(), ports.end(), p));
	delete p;
	resize();
}

typedef std::map<std::string, std::map<std::string, DM::View> > stream_map;
typedef std::map<std::string, DM::View> view_map;


void ModelNode::paint_rect(QPainter *painter)
{
	if(isSelected() || hovered)
		setZValue(5.0); //Above Link
	else
		setZValue(2.0);

	float lineWidth = 1.0f;
	QColor fillcolor = COLOR_MOD_BG_COLOR;
	QColor border_color = COLOR_MODULEBORDER; //COLOR_MODULEBORDER;



	/*if(module->isSuccessorMode() && (module->getStatus() == DM::MOD_EXECUTION_OK || module->getStatus() == DM::MOD_CHECK_OK) )
	{
		fillcolor = QColor(254,221,86);
	}*/

	if(isSelected() || hovered)
	{
		border_color = QColor(0,0,255);
		lineWidth = 2.0f;
	}

	// the constructor for QColor is neccessary
	QPen rectPen(QColor(border_color), lineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	QBrush brush(fillcolor);

	QPainterPath rectPath;
	rectPath.addRect(boundingRect().adjusted(lineWidth,lineWidth,-lineWidth,-lineWidth));
	painter->fillPath(rectPath, brush);
	painter->strokePath(rectPath, rectPen);
}

double ModelNode::paint_header(QPainter * painter, double & pos) {

	// Set Module Class

	// the constructor for QColor is neccessary
	//QPen rectPen(QColor(254,221,86), 1.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	float lineWidth = 1.0f;
	QColor fillcolor;
	switch(module->getStatus())
	{
	case DM::MOD_UNTOUCHED:			fillcolor = COLOR_MOD_UNTOUCHED;			break;

	case DM::MOD_EXECUTING:			fillcolor = COLOR_MOD_EXECUTING;			break;
	case DM::MOD_EXECUTION_OK:		fillcolor = COLOR_MOD_EXECUTION_OK;			break;
	case DM::MOD_EXECUTION_ERROR:	fillcolor = COLOR_MOD_EXECUTION_ERROR;		break;

	case DM::MOD_CHECK_OK:			fillcolor = COLOR_MOD_CHECK_OK;				break;
	case DM::MOD_CHECK_ERROR:		fillcolor = COLOR_MOD_CHECK_ERROR;			break;
	}

	if(isSelected() || hovered)
	{
		lineWidth = 2.0f;
	}
	QBrush brush(fillcolor);
	QPainterPath rectPath;
	rectPath.addRect(boundingRect().x() + 2*lineWidth, boundingRect().y() + 2*lineWidth, boundingRect().width()-4*lineWidth, pos + 27 - 2*lineWidth );
	painter->fillPath(rectPath, brush);
	//painter->strokePath(rectPath, rectPen);

	QString text =  QString::fromStdString(module->getClassName());
	QRectF textSize_Name = QGraphicsSimpleTextItem(text).boundingRect();
	QFont font = painter->font();
	painter->setFont(font);
	painter->setPen(COLOR_MODLE_TEXT_COLOR);
	// Set Name
	painter->drawText(	10,
						textSize_Name.height() + 0,
						text);

	QString text_m =  QString::fromStdString(module->getName());
	QRectF textSize_m = QGraphicsSimpleTextItem(text_m).boundingRect();
	painter->setFont(font);


	painter->setPen(COLOR_MODLE_TEXT_COLOR);
	painter->drawText(	20 + textSize_Name.width(),
						textSize_m.height() + 0,
						text_m);
	width = max(width, (int)textSize_Name.width() + (int)textSize_m.width()+ 25);

	return pos + 25;
}


double ModelNode::paint_datastream(QPainter *painter, double & pos)
{
	stream_map viewsInStream = this->module->getAccessedViews();
	if (viewsInStream.size() == 0) {
		return pos;
	}
	pos = pos + 12;
	int counter = 0;
	painter->setFont(QFont(QFont().defaultFamily(), 10, QFont::Bold));
	painter->drawText(	10, pos + 13* counter,
						"Data-stream");
	counter++;



	for (stream_map::const_iterator it = viewsInStream.begin(); it != viewsInStream.end(); ++it)
	{
		mforeach (const DM::View& v, it->second)
		{
			if (v.getName() == "dummy")
				continue;


			bool view_missing = false;
			if (v.reads() && !map_contains(&this->module->getViewsInStream()[it->first], v.getName()))
				view_missing = true;


			int type = v.getType();
			std::stringstream datastream;
			datastream << v.getName() << "\t";
			switch(v.getType())
			{
			case DM::NODE:			datastream <<  "Nodes\t";			break;
			case DM::EDGE:			datastream <<  "Edges\t";			break;
			case DM::FACE:			datastream <<  "Faces\t";			break;
			case DM::SUBSYSTEM:		datastream <<  "Systems\t";		break;
			case DM::RASTERDATA:	datastream <<  "Raster Data\t";	break;
			case DM::COMPONENT:		datastream <<  "Component\t";		break;
			}

			switch(v.getAccessType())
			{
			case DM::WRITE:		datastream <<  "write";		break;
			case DM::READ:		datastream << "read";		break;
			case DM::MODIFY:	datastream <<  "modify";	break;
			}

			QString textdata =  QString::fromStdString(datastream.str());

			width = max(width,  (int)QGraphicsSimpleTextItem(textdata).boundingRect().width()+ 10);

			painter->setPen(COLOR_MODLE_TEXT_COLOR);
			if (view_missing)
				painter->setPen(COLOR_MODULE_COLOR_ERROR);

			painter->setFont(QFont(QFont().defaultFamily(), 10, QFont::Bold));
			painter->drawText(	12, pos + 13* counter,
								textdata);
			counter++;
			foreach(std::string s, v.getAllAttributes())
			{
				std::stringstream attribute_stream;
				attribute_stream << s << "\t";
				bool attribute_missing = false;
				QString typeString;
				switch(v.getAttributeAccessType(s))
				{
				case DM::READ:
					if (view_missing || !this->module->getViewsInStream()[it->first][v.getName()].hasAttribute(s))
					{
						//any_attribute_missing = true;
						attribute_missing = true;
					}
					typeString = "read";
					break;
				case DM::MODIFY:	typeString = "modify";	break;
				case DM::WRITE:		typeString = "write";	break;
				}

				switch(v.getAttributeType(s))
				{
				case DM::Attribute::DOUBLE:
					attribute_stream << "double"; break;
				case DM::Attribute::DOUBLEVECTOR:
					attribute_stream << "doublevector"; break;
				case DM::Attribute::LINK:
					attribute_stream <<  "link"; break;
				case DM::Attribute::NOTYPE:
					attribute_stream <<  "no type"; break;
				case DM::Attribute::STRING:
					attribute_stream <<  "string"; break;
				case DM::Attribute::STRINGVECTOR:
					attribute_stream <<  "stringvector"; break;
				case DM::Attribute::TIMESERIES:
					attribute_stream <<  "time series"; break;
				}

				attribute_stream  << "\t" << typeString.toStdString() ;
				textdata =  QString::fromStdString(attribute_stream.str());

				width = max(width,  (int)QGraphicsSimpleTextItem(textdata).boundingRect().width() + 16);
				painter->setPen(COLOR_MODLE_TEXT_COLOR);
				if (attribute_missing)
					painter->setPen(COLOR_MODULE_COLOR_ERROR);
				painter->setFont(QFont(QFont().defaultFamily(), 10, QFont::Normal));
				painter->drawText(	14, pos + 13* counter,
									textdata);
				counter++;
			}
		}
	}

	return (counter*13) + pos;
}

double ModelNode::paint_parameter(QPainter *painter, double & pos)
{


	pos = pos + 12;
	int counter = 0;

	painter->setFont(QFont(QFont().defaultFamily(), 10, QFont::Bold));
	painter->drawText(	10, pos + 13* counter,
						"Parameter");
	counter++;



	foreach (DM::Module::Parameter * p, this->module->getParameters()) {

		std::stringstream parameter;
		parameter << p->name;
		parameter << "\t";
		switch(p->type)
		{
		case DM::INT:		parameter <<  *(int*)		p->data;	break;
		case DM::LONG:		parameter <<  *(long*)	p->data;	break;
		case DM::DOUBLE:	parameter <<  *(double*)	p->data ;break;
		case DM::BOOL:
			if (*(bool*)	p->data)
				parameter << "true";
			else
				parameter << "false";
			break;
		case DM::FILENAME:
		case DM::STRING:	parameter <<  *(std::string*)	p->data;	break;

		case DM::STRING_LIST:
			{
				QString textdata = QString::fromStdString(parameter.str());
				painter->setFont(QFont(QFont().defaultFamily(), 10, QFont::Normal));
				painter->drawText(	12, pos + 13* counter,
									textdata);
				counter++;
				foreach (std::string s, *(std::vector<std::string>*)p->data) {
					std::stringstream ss;
					ss << " - ";
					ss << s;
					textdata = QString::fromStdString(ss.str());
					painter->setFont(QFont(QFont().defaultFamily(), 10, QFont::Normal));
					painter->drawText(	14, pos + 13* counter,
										textdata);
					counter++;
				}
				continue;
			}
			break;

		case DM::STRING_MAP:
			{
				std::map<std::string,std::string> var_map = *(std::map<std::string,std::string>*)p->data;
				QString textdata = QString::fromStdString(parameter.str());
				painter->setFont(QFont(QFont().defaultFamily(), 10, QFont::Normal));
				painter->drawText(	12, pos + 13* counter,
									textdata);
				counter++;
				for (std::map<std::string,std::string>::const_iterator it = var_map.begin(); it != var_map.end(); ++it) {
					std::stringstream ss;
					ss << " - ";
					ss << it->first;
					ss << "\t";
					ss << it->second;
					textdata = QString::fromStdString(ss.str());
					painter->setFont(QFont(QFont().defaultFamily(), 10, QFont::Normal));
					painter->drawText(	14, pos + 13* counter,
										textdata);
					counter++;
				}
				continue;
			}
			break;
		}


		QString textdata = QString::fromStdString(parameter.str());
		painter->setPen(COLOR_MODLE_TEXT_COLOR);

		painter->setFont(QFont(QFont().defaultFamily(), 10, QFont::Normal));
		painter->drawText(	12, pos + 13* counter,
							textdata);

		width = max(width,  (int)QGraphicsSimpleTextItem(textdata).boundingRect().width() + 14);
		counter++;

	}

	return (counter*13) + pos;
}


double ModelNode::paint_filter(QPainter *painter, double & pos)
{
	if ( this->module->getFilter().size() == 0 )
		return pos;

	pos = pos + 12;
	int counter = 0;

	painter->setFont(QFont(QFont().defaultFamily(), 10, QFont::Bold));
	painter->drawText(	10, pos + 13* counter,
						"Filter");
	counter++;

	foreach (DM::Filter f, this->module->getFilter()) {

		std::stringstream filter;
		//filter << f.getViewName();
		painter->setFont(QFont(QFont().defaultFamily(), 10, QFont::Bold));
		painter->drawText(	12, pos + 13* counter,
							QString::fromStdString(f.getViewName()));
		counter++;
		if (!f.getAttributeFilter().getArgument().empty())
			filter << " attribute: " << f.getAttributeFilter().getArgument();
		if (!f.getSpatialFilter().getArgument().empty())
			filter<< " spatial: " << f.getSpatialFilter().getArgument();

		QString textdata = QString::fromStdString(filter.str());
		painter->setPen(COLOR_MODLE_TEXT_COLOR);

		painter->setFont(QFont(QFont().defaultFamily(), 10, QFont::Normal));
		painter->drawText(	12, pos + 13* counter,
							textdata);

		width = max(width,  (int)QGraphicsSimpleTextItem(textdata).boundingRect().width() + 14);
		counter++;
	}

	return (counter*13) + pos;
}


void ModelNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	double pos = 0;

	paint_rect(painter);

	// Header
	pos = paint_header(painter, pos);

	// Parameter
	pos = paint_parameter(painter, pos);
	// Filter
	pos = paint_filter(painter, pos);
	// Data stream
	pos = paint_datastream(painter, pos);

	this->height = max(height, (int) pos);
	this->resize();
}



QRectF ModelNode::boundingRect() const
{
	return QRect (0, 0, width, height);
}

void ModelNode::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
{
	if(DM::Module* m = module->getOwner())
	{
		if(GroupNode* gn = (GroupNode*)getSimulation()->getModelNode(m)->getChild())
		{
			gn->resize();
			this->scene()->update();
		}
	}
	QGraphicsItem::mouseMoveEvent(event);
}

void ModelNode::mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * event )
{
	editModelNode();
}

void ModelNode::hoverEnterEvent( QGraphicsSceneHoverEvent * event )
{
	hovered = true;
	this->update();
	QGraphicsItem::hoverEnterEvent(event );
}
void ModelNode::hoverLeaveEvent( QGraphicsSceneHoverEvent * event )
{
	hovered = false;
	this->update();
	QGraphicsItem::hoverLeaveEvent(event );
}

void ModelNode::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
	QMenu menu;

	// out port viewer
	QSignalMapper* signalMapper = new QSignalMapper(this);
	foreach(string s, module->getOutPortNames())
	{
		QAction* action = menu.addAction(QString::fromStdString("view data at out port '"+s+"'"));
		connect( action, SIGNAL(triggered() ), signalMapper, SLOT( map() ));
		signalMapper->setMapping(action, QString::fromStdString(s));

		if (!module->getOutPortData(s))
			action->setDisabled(true);
	}
	connect(signalMapper, SIGNAL(mapped(QString)), this, SLOT(viewOutportData(QString)));

	// in port viewer
	signalMapper = new QSignalMapper(this);
	foreach(string s, module->getInPortNames())
	{
		QAction* action = menu.addAction(QString::fromStdString("view data at in port '"+s+"'"));
		connect( action, SIGNAL(triggered() ), signalMapper, SLOT( map() ));
		signalMapper->setMapping(action, QString::fromStdString(s));

		if (!module->getInPortData(s))
			action->setDisabled(true);
	}
	connect(signalMapper, SIGNAL(mapped(QString)), this, SLOT(viewInportData(QString)));

	if(module->getOutPortNames().size()+module->getInPortNames().size() > 0)
		menu.addSeparator();

	QAction* a_edit = menu.addAction("configurate");
	QAction* a_editName = menu.addAction("edit name");
	QAction* a_filter = menu.addAction("edit filter");
	QAction* a_delete = menu.addAction("delete");
	QAction* a_successorMode = menu.addAction("force successor mode");
	menu.addSeparator();
	QAction* a_showHelp = menu.addAction("show help");
	QAction* a_showData = menu.addAction("show data stream");

	connect( a_edit, SIGNAL( triggered() ), this, SLOT( editModelNode() ), Qt::DirectConnection );
	connect( a_editName, SIGNAL( triggered() ), this, SLOT( editName() ), Qt::DirectConnection );
	connect( a_filter, SIGNAL( triggered() ), this, SLOT( editFilter() ), Qt::DirectConnection );
	connect( a_delete, SIGNAL( triggered() ), this, SLOT( deleteModelNode() ), Qt::DirectConnection );
	connect( a_showHelp, SIGNAL(triggered() ), this, SLOT( showHelp() ), Qt::DirectConnection);
	connect( a_showData, SIGNAL(triggered() ), this, SLOT( printData() ), Qt::DirectConnection);

	connect( a_successorMode, SIGNAL(triggered() ), this, SLOT( changeSuccessorMode() ), Qt::DirectConnection);

	menu.exec(event->screenPos());
}

void ModelNode::editModelNode()
{
	if(!module->createInputDialog())
	{
		if(!ng)
			delete ng;

		ng = new GUIModelNode(module ,this);
		connect(ng,SIGNAL(updateStream()),this,SLOT(updateSimulation()),Qt::DirectConnection);
		ng->show();
	}
}

void ModelNode::editName()
{
	bool ok;
	QString text = QInputDialog::getText(0, "set name", "specify name of this module",
										 QLineEdit::Normal, QString::fromStdString(module->getName()), &ok);
	if (ok && !text.isEmpty())
		module->setName(text.toStdString());
	this->resize();
}

void ModelNode::editFilter()
{
	GUIFilter * gui_f = new GUIFilter(this->module);
	gui_f->show();
}

void ModelNode::deleteModelNode()
{
	getSimulation()->removeModule(getModule());
}

void ModelNode::printData()
{
	GUIViewDataForModules * gv = new GUIViewDataForModules(module);
	gv->show();
}

void ModelNode::changeSuccessorMode()
{
	module->setSuccessorMode(!module->isSuccessorMode());
}

void ModelNode::updateSimulation()
{
	this->getSimulation()->reset();
}

void ModelNode::viewOutportData(QString portName)
{
	std::map<std::string, DM::View> views;
	std::map<std::string, std::map<std::string, DM::View> > stream = module->getViewsInOutStream();
	map_contains(&stream, portName.toStdString(), views);

	DM::System *sys = dynamic_cast<DM::System*>(module->getOutPortData(portName.toStdString()));

	if(!sys)
	{
		DM::Logger(DM::Error) << "Cannot view this type of system: PLEASE IMPLEMENT ME";
		return;
	}
#ifdef VIEWER
	DM::ViewerWindow *viewer_window = new DM::ViewerWindow(sys, views);
	viewer_window->show();
#endif
}

void ModelNode::viewInportData(QString portName)
{
	std::map<std::string, DM::View> views;
	std::map<std::string, std::map<std::string, DM::View> > stream = module->getViewsInOutStream();
	map_contains(&stream, portName.toStdString(), views);

	DM::System *sys = dynamic_cast<DM::System*>(module->getInPortData(portName.toStdString()));

	if(!sys)
	{
		DM::Logger(DM::Error) << "Cannot view this type of system: PLEASE IMPLEMENT ME";
		return;
	}
#ifdef VIEWER
	DM::ViewerWindow *viewer_window = new DM::ViewerWindow(	sys, views);
	viewer_window->show();
#endif
}

void ModelNode::showHelp()
{
	GUIHelpViewer* ghv = new GUIHelpViewer(simulation);
	ghv->showHelpForModule(module);
	ghv->show();
}
