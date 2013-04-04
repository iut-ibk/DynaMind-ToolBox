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
#include "guiport.h"
#include <modelnode.h>
#include <dmport.h>
#include "ColorPalette.h"
#include <guilink.h>
#include <QGraphicsSceneMouseEvent>
#include <dmsimulation.h>
#include <dmmodulelink.h>
#include <guisimulation.h>
PortNode::~PortNode () {

    foreach(GUILink *l, this->links) {
        delete l;
        l = 0;
    }
    this->links.clear();
}
void PortNode::removeLink(GUILink * l) {
    int index = this->links.indexOf(l);
    if (index > -1) {
        this->links.remove(index);
        DM::Logger(DM::Debug) << "Remove GUILink from" << this->getPortName() << this->links.size();
    }
}
/*
void PortNode::updatePort(DM::Port * p) {
    this->p = p;
}*/
PortNode::PortNode(QString portName, ModelNode *parentModelNode, PortType type/*, DM::Port *p*/) : QGraphicsItem(parentModelNode)
{
    this->setParentItem(parentModelNode);
    this->setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);
    this->setAcceptHoverEvents(true);
    this->setAcceptsHoverEvents(true);
    this->PortName = portName;
    tmp_link = 0;
    //this->hoverElement = 0;
    //this->p = p;
    this->x1 = 0;
    this->isHover = false;
    this->LinkMode = false;
    this->modelNode = parentModelNode;
    this->portType = type;
	if(type == INPORT)
		this->setPos(-7,7);
	else
		this->setPos(-7+parentModelNode->boundingRect().width(),7);

	


    this->simulation = parentModelNode->getSimulation();
    //this->simpleTextItem = new QGraphicsSimpleTextItem (QString::fromStdString(p->getLinkedDataName()));

    //if (p->getPortType() == DM::INSYSTEM || p->getPortType() == DM::OUTSYSTEM)
        color = COLOR_VECTORPORT;

    hoverElement = 0;


}
bool PortNode::isLinked() {
    //if (this->getVIBePort()->getLinks().size() > 0)
        return true;
    return false;
}

void PortNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    //if (this->getVIBePort()->isFullyLinked())
        color = Qt::green;
    //if (!this->getVIBePort()->isFullyLinked())
        color = Qt::red;
    painter->setBrush(color);
	
    QPainterPath path;
	QPen pen(Qt::black, 0.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    if(!isHover)
        path.addEllipse(0, 0,PORT_DRAW_SIZE,PORT_DRAW_SIZE);
	else 
	{
		pen.setWidth(1);
        path.addEllipse(-1, -1,PORT_DRAW_SELECTED_SIZE,PORT_DRAW_SELECTED_SIZE);
    }
	painter->fillPath(path, color);
	painter->strokePath(path, pen);

    portname_graphics.setText(this->getPortName());

    //if (this->getPortType() > DM::OUTPORTS)
	if(portType == INPORT)
        painter->drawText(QPoint(10,portname_graphics.boundingRect().height()/2+10), this->getPortName());
	else
    //if (this->getPortType() < DM::OUTPORTS)
        painter->drawText(QPoint(-portname_graphics.boundingRect().width()-10,portname_graphics.boundingRect().height()/2+10), this->getPortName());
    painter->setBrush(Qt::NoBrush);
}

QRectF PortNode::boundingRect() const
{
    /*if(portType == INPORT)
        return QRect(-10, 10,20+this->portname_graphics.boundingRect().width(),20);
    else
        return QRect(-10-this->portname_graphics.boundingRect().width(), 10,20+this->portname_graphics.boundingRect().width(),20);
	*/

	return QRect(0,0,PORT_DRAW_SIZE,PORT_DRAW_SIZE);
}

void PortNode::hoverEnterEvent ( QGraphicsSceneHoverEvent * event ) {
    this->isHover = true;
	/*
    //if (this->PortType  == DM::INSYSTEM||this->PortType  == DM::OUTSYSTEM)
        color = COLOR_VECTORPORT;
		*/
    prepareGeometryChange ();/*
    l = this->portname_graphics.boundingRect().width()+4;

    h = this->portname_graphics.boundingRect().height()+4;
    x1 = 0;
    if (portType == OUTPORT  )
        x1 = -l+14;
    this->update(this->boundingRect());*/
}

void PortNode::hoverLeaveEvent ( QGraphicsSceneHoverEvent * event ) {
    this->isHover = false;
    /*if (!LinkMode) {

        //if (PortType  == DM::INSYSTEM || PortType == DM::OUTSYSTEM)
            color = COLOR_VECTORPORT;

    }*/
    prepareGeometryChange ();/*
    this->update(this->boundingRect());*/
}

QPointF PortNode::getCenterPos() 
{
    return  QPointF( this->scenePos() + QPointF(PORT_DRAW_SIZE,PORT_DRAW_SIZE)/2);
}

/*int PortNode::getPortType() {
    return this->PortType;
}*/

static GUILink* unstableLink = NULL;

void PortNode::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )  {
    this->scene()->sendEvent(0, event);

	//DM::Logger(DM::Debug) << "PortNode::mouseMoveEvent";
	if(unstableLink)
	{
		if(portType == INPORT)
			unstableLink->setOutPort(event->scenePos());
		else
			unstableLink->setInPort(event->scenePos());
	}


	/*
    if (LinkMode) {
        this->tmp_link->setInPort(event->scenePos());
    }
    QList<QGraphicsItem  *> items = this->scene()->items(event->scenePos());
    //Check Hover Event
    bool setHover = false;
    foreach (QGraphicsItem  * item, items) {
        if ( this->type() == item->type() ) {
            PortNode * link  = (PortNode *) item;

            //if (getPortType() == DM::OUTSYSTEM &&  link->getPortType() == DM::INSYSTEM ) {
                link->setHover(true);
                link->prepareGeometryChange();
                link->update();
                this->hoverElement = link;
                setHover = true;
            //}
        }
    }
    if(!setHover) {
        if(this->hoverElement != 0) {
            this->hoverElement->setHover(false);
        }
        this->hoverElement = 0;
    }*/
}


void PortNode::mousePressEvent ( QGraphicsSceneMouseEvent * event )  
{
	DM::Logger(DM::Debug) << "PortNode::mousePressEvent";

	if(unstableLink)
	{
		DM::Logger(DM::Warning) << "deleting lost link";
		delete unstableLink;
	}

	unstableLink = new GUILink();

	if(portType == INPORT)
		unstableLink->setInPort(this);
	else
		unstableLink->setOutPort(this);

	this->scene()->addItem(unstableLink);
	
	/*std::cout << "Mouse Press event" << std::endl;
    //If currently in link mode delete node and proceed with other stuff
    if (LinkMode ) {
        if (this->tmp_link != 0) {
            delete this->tmp_link;
            this->tmp_link = 0;
        }
        LinkMode = false;
        return;
    }


    //if (getPortType() == DM::INSYSTEM || getPortType() == DM::OUTSYSTEM )
        color = COLOR_VECTORPORT;



    //if (getPortType() < DM::OUTPORTS) {
        LinkMode = true;
        this->tmp_link = new GUILink();
        this->tmp_link->setOutPort(this);
        this->scene()->addItem(this->tmp_link);
    //}
		*/
}
/*
DM::Port * PortNode::getVIBePort() {
    DM::Port * p = 0;
    if (this->PortType > DM::OUTPORTS) {
        p = this->modelNode->getDMModel()->getInPort(this->getPortName().toStdString());
    } else {
        p = this->modelNode->getDMModel()->getOutPort(this->getPortName().toStdString());
    }
   return p;
}*/

void PortNode::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event ) 
{
	DM::Logger(DM::Debug) << "PortNode::mouseReleaseEvent";

	QList<QGraphicsItem*> items = this->scene()->items(event->scenePos());
	foreach (QGraphicsItem  * item, items)
	{
		if(PortNode* port = dynamic_cast<PortNode*>(item))
		{
			if(port->portType == INPORT)
				unstableLink->setInPort(port);
			else
				unstableLink->setOutPort(port);

			// one of them is the current port, but instead of checking
			// we just use the method - just laziness/good coding style
			unstableLink->getInPort()->links.append(unstableLink);
			unstableLink->getOutPort()->links.append(unstableLink);

			unstableLink = NULL;
			break;
		}
	}

	/*
    //if (getPortType() < DM::OUTPORTS) {
        LinkMode = false;

        //if (getPortType()  == DM::INSYSTEM)
            color = COLOR_VECTORPORT;

        this->update(this->boundingRect());
        QList<QGraphicsItem*> items = this->scene()->items(event->scenePos());
        bool newLink = false;
        foreach (QGraphicsItem  * item, items) 
		{
            if ( this->type() == item->type() ) 
			{
                PortNode * endLink  = (PortNode *) item;

                //if (getPortType() == DM::OUTSYSTEM &&  endLink->getPortType() == DM::INSYSTEM ) {
                    this->tmp_link->setInPort(endLink);
                    //this->links.append(tmp_link);
                    //Create Link
                    //tmp_link->setVIBeLink(this->modelNode->getSimulation()->addLink(tmp_link->getOutPort()->getVIBePort(), tmp_link->getInPort()->getVIBePort()));
                    tmp_link->setSimulation(this->modelNode->getSimulation());
                    newLink = true;
                    tmp_link = 0;

                    //Run Simulation
                    this->modelNode->getSimulation()->updateSimulation();
                }
            }
        }
		*/
	
	if(unstableLink)
	{
		delete unstableLink;
		unstableLink = NULL;
	}
	/*
        if (!newLink ) {
            if (this->tmp_link != 0) {
                delete this->tmp_link;
                this->tmp_link = 0;
            }
        }
    //}
	*/
}
void PortNode::refreshLinks() 
{
	/*
	foreach(GUILink * l, this->links) {
        if ( l != 0) {
            l->refresh();
        }
    }*/
}
void PortNode::setLink(GUILink * l) 
{
/*
    int index = this->links.indexOf(l);
    if (index == -1) {
        this->links.append(l);
        DM::Logger(DM::Debug) << "Set Link" << this->getPortName();
    }*/

}
QVariant PortNode::itemChange(GraphicsItemChange change, const QVariant &value) {
    if(change == QGraphicsItem::ItemScenePositionHasChanged) {
        this->refreshLinks();
    }
    /*if (change == QGraphicsItem::ItemVisibleHasChanged) {
        foreach(GUILink * l, this->links)
            l->setVisible(this->isVisible());
    }*/
    return QGraphicsItem::itemChange(change, value);
}




