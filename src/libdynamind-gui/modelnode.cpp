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
#include <guiport.h>
#include <guisimulation.h>
#include <dmsystem.h>
#include <dmviewerwindow.h>
#include <groupnode.h>

#include <guihelpviewer.h>
#include <qdialog.h>

void GUIModelObserver::notifyAddPort(const std::string &name, const DM::PortType type)
{
	node->addPort(name, type);
}
void GUIModelObserver::notifyRemovePort(const std::string &name, const DM::PortType type)
{
	node->removePort(name, type);
}

ModelNode::ModelNode(DM::Module* m, GUISimulation* sim)
{
	child = NULL;
	module = m;

	setAcceptDrops(true);

	simulation = sim;
    this->setFlag(QGraphicsItem::ItemIsSelectable, true);
    this->setFlag(QGraphicsItem::ItemIsMovable, true);
    this->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

	resize();

	// be shure to add observer last, as module dimensions are not set before
	new GUIModelObserver(this, m);

	hovered = false;
	setAcceptHoverEvents(true);
}

void ModelNode::resize()
{
	// module name has to fit in node
    QString text = QString::fromStdString(module->getName());
	QRectF textSize = QGraphicsSimpleTextItem(text).boundingRect();
    width = max(50, (int)textSize.width() + 30);
	
	// make groups a bit bigger
	if(!module->isGroup())
		height =  45;
	else
		height =  65;

	// ports names have to fit
    /*int maxPortSize = 0;
	foreach(PortNode* p, ports)
		maxPortSize = max(maxPortSize, (int)QGraphicsSimpleTextItem(p->getPortName()).boundingRect().width());

	maxPortSize += 20;
    width = max(width, 2*maxPortSize);*/
	
	// port number has to fit
	int maxPortCount = max(module->getInPortNames().size(), module->getOutPortNames().size());
	height = max(height, 20 + 15 * maxPortCount);

	// update port pos
	foreach(PortNode* p, ports)
		p->updatePos();
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

void ModelNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) 
{

    if(isSelected() || hovered)
        setZValue(5.0); //Above Link
	else
        setZValue(2.0);

	float lineWidth = 2.0f;
	QColor fillcolor;    

	switch(module->getStatus())
	{
    case DM::MOD_UNTOUCHED:			fillcolor = QColor(200,198,187);	break;

    case DM::MOD_EXECUTING:			fillcolor = QColor(254,225,104);		break;
    case DM::MOD_EXECUTION_OK:		fillcolor = QColor(153,204,255);		break;
    case DM::MOD_EXECUTION_ERROR:	fillcolor = QColor(255,153,51);		break;

    case DM::MOD_CHECK_OK:			fillcolor = QColor(255,255,255);	break;
    case DM::MOD_CHECK_ERROR:		fillcolor = QColor(255,153,51);	break;
	}

	// the constructor for QColor is neccessary
	QPen rectPen(QColor(COLOR_MODULEBORDER), lineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	QBrush brush(fillcolor);

	QPainterPath rectPath;
	rectPath.addRect(boundingRect().adjusted(lineWidth,lineWidth,-lineWidth,-lineWidth));
	painter->fillPath(rectPath, brush);
    painter->strokePath(rectPath, rectPen);

	if(module->isSuccessorMode())
	{
        QPainterPath rectGlowPath;
        float l = lineWidth*2;
        QRectF r = boundingRect().adjusted(l,l,-l,-l);
        rectGlowPath.addRect(r);
        painter->strokePath(rectGlowPath,
            QPen(QColor(200,198,187), lineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
	}

	if(isSelected() || hovered)
	{
		QPainterPath rectGlowPath;
		QRectF r = boundingRect();
		rectGlowPath.addRect(r);
		painter->strokePath(rectGlowPath, 
            QPen(isSelected()?QColor(0,0,0):QColor(200,198,187),
			lineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
	}

    QString text =  QString::fromStdString(module->getName());
	QRectF textSize = QGraphicsSimpleTextItem(text).boundingRect();
    QFont font = painter->font();
    painter->setFont(font);
    painter->setPen(QColor(0,0,0));
	painter->drawText(	(boundingRect().width() - textSize.width())/2,
		textSize.height() + 0,
		text);


	this->update();
}


QRectF ModelNode::boundingRect() const 
{
    QRect r (0, 0,
             width, height);
    return r;

}

QVariant ModelNode::itemChange(GraphicsItemChange change, const QVariant &value) 
{
    return QGraphicsItem::itemChange(change, value);
}

ModelNode::~ModelNode() 
{
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
	QSignalMapper* signalMapper = new QSignalMapper(this);
	int i=0;
	foreach(string s, module->getOutPortNames())
	{
		QAction* action = menu.addAction(QString::fromStdString("view data at port '"+s+"'"));
		connect( action, SIGNAL(triggered() ), signalMapper, SLOT( map() ));

		signalMapper->setMapping(action, i++);
	}
	connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(viewData(int)));

	if(module->getOutPortNames().size())
		menu.addSeparator();
	
    QAction* a_edit = menu.addAction("configurate");
    QAction* a_editName = menu.addAction("edit name");
    QAction* a_delete = menu.addAction("delete");
    QAction* a_successorMode = menu.addAction("force successor mode");
	menu.addSeparator();
    QAction* a_showHelp = menu.addAction("show help");
    QAction* a_showData = menu.addAction("show data stream");
	
    connect( a_edit, SIGNAL( triggered() ), this, SLOT( editModelNode() ), Qt::DirectConnection );
    connect( a_editName, SIGNAL( triggered() ), this, SLOT( editName() ), Qt::DirectConnection );
    connect( a_delete, SIGNAL( triggered() ), this, SLOT( deleteModelNode() ), Qt::DirectConnection );
    connect( a_showHelp, SIGNAL(triggered() ), this, SLOT( showHelp() ), Qt::DirectConnection);
    connect( a_showData, SIGNAL(triggered() ), this, SLOT( printData() ), Qt::DirectConnection);

	connect( a_successorMode, SIGNAL(triggered() ), this, SLOT( changeSuccessorMode() ), Qt::DirectConnection);

    menu.exec(event->screenPos());
}

void ModelNode::editModelNode() 
{
	if(!module->createInputDialog())
		(new GUIModelNode(module ,this))->show();
}

void ModelNode::editName() 
{
	module->setName(QInputDialog::getText(0, "set name", "specify name of this module", 
		QLineEdit::Normal, QString::fromStdString(module->getName())).toStdString());
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

void ModelNode::viewData(int portIndex) 
{
	DM::System *system = module->getOutPortData(module->getOutPortNames()[portIndex]);
    DM::ViewerWindow *viewer_window = new DM::ViewerWindow(system);
    viewer_window->show();
}

void ModelNode::showHelp() 
{
	GUIHelpViewer* ghv = new GUIHelpViewer(simulation);
	ghv->showHelpForModule(module);
    ghv->show();
}
