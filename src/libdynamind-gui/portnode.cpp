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

#include "portnode.h"
#include <modelnode.h>
#include "ColorPalette.h"
#include <linknode.h>
#include <QGraphicsSceneMouseEvent>
#include <dmsimulation.h>
#include <guisimulation.h>
#include <dmlogger.h>

PortNode::PortNode(QString portName, DM::Module * m, DM::PortType type, 
				   QGraphicsItem* parent, GUISimulation* simulation)
				   :QGraphicsItem(parent), portLabel(parent)
{
	this->setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);
	this->setAcceptHoverEvents(true);
	this->setAcceptsHoverEvents(true);
	this->portName = portName;

	unstableLink = NULL;
	this->isHover = false;
	this->module = m;
	this->portType = type;
	this->updatePos();

	this->simulation = simulation;

	portLabel.setText(portName);
	portLabel.setVisible(false);
	portLabel.setPos(this->pos());

	if(portType == DM::INPORT)	portLabel.moveBy( boundingRect().width() + 3 , 0);
	else						portLabel.moveBy( -portLabel.boundingRect().width() - 3, 0);

}

PortNode::~PortNode () {

	foreach(LinkNode *l, this->linkNodes) {
		delete l;
		l = 0;
	}
	this->linkNodes.clear();
}

void PortNode::removeLink(LinkNode * l)
{
	int index = this->linkNodes.indexOf(l);
	if (index > -1) {
		this->linkNodes.remove(index);
		DM::Logger(DM::Debug) << "Remove LinkNode from port '" << this->getPortName() << "' ," << this->linkNodes.size() << " left";
	}
}

void PortNode::addLink(LinkNode* l)
{
	linkNodes.append(l);
}

void PortNode::setHover(bool b)
{
	this->isHover = b;
	this->prepareGeometryChange();
}

void PortNode::updatePos()
{	
	std::vector<std::string> ports;
	if(DM::INPORT == portType)
		ports = module->getInPortNames();
	else if(DM::OUTPORT == portType)
		ports = module->getOutPortNames();

	std::vector<std::string>::iterator it = std::find(ports.begin(), ports.end(), portName.toStdString());

	int portIndex = it - ports.begin();
	int xoffset = -7;
	int yoffset = 10 + 15*portIndex;

	if(portType == DM::INPORT)
		this->setPos(xoffset, yoffset);
	else
		this->setPos(xoffset + this->parentItem()->boundingRect().width(), yoffset);
}

void PortNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) 
{
	QColor color;
	if(module && (portType == DM::OUTPORT && module->getOutPortData(portName.toStdString())))
		color = Qt::yellow;
	else if(linkNodes.size() || unstableLink)
		color = Qt::green;
	else
		color = Qt::red;

	QPainterPath path;
	QPen pen(Qt::black, 0.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	if(!isHover)
		path.addEllipse(0, 0,PORT_DRAW_SIZE,PORT_DRAW_SIZE);
	else
		path.addEllipse(-1, -1,PORT_DRAW_SELECTED_SIZE,PORT_DRAW_SELECTED_SIZE);

	painter->setBrush(color);
	painter->fillPath(path, color);
	painter->strokePath(path, pen);

	portLabel.setVisible(isHover);
}

QRectF PortNode::boundingRect() const
{
	return QRect(0,0,PORT_DRAW_SIZE,PORT_DRAW_SIZE);
}

void PortNode::hoverEnterEvent ( QGraphicsSceneHoverEvent * event ) 
{
	setHover(true);
	QGraphicsItem::hoverEnterEvent(event);
}

void PortNode::hoverLeaveEvent ( QGraphicsSceneHoverEvent * event ) 
{
	setHover(false);
	QGraphicsItem::hoverLeaveEvent(event);
}

QPointF PortNode::getCenterPos() const
{
	return QPointF( this->scenePos() + QPointF(PORT_DRAW_SIZE,PORT_DRAW_SIZE)/2);
}

QVariant PortNode::itemChange(GraphicsItemChange change, const QVariant &value) 
{
	if(change == QGraphicsItem::ItemScenePositionHasChanged) 
		foreach(LinkNode* link, linkNodes)
		link->refresh();
	return QGraphicsItem::itemChange(change, value);
}

void PortNode::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
{
	if(unstableLink)
	{
		if(portType == DM::INPORT)
			unstableLink->setOutPort(event->scenePos());
		else
			unstableLink->setInPort(event->scenePos());
	}
	// reset all port hovers
	QList<QGraphicsItem  *> items = this->scene()->items();
	foreach (QGraphicsItem  * item, items)
        if(PortNode* port = dynamic_cast<PortNode*>(item))
			port->setHover(false);

	// Check Hover Event
	items = this->scene()->items(event->scenePos());
	foreach (QGraphicsItem  * item, items)
        if(PortNode* port = dynamic_cast<PortNode*>(item))
			port->setHover(true);
}

void PortNode::mousePressEvent ( QGraphicsSceneMouseEvent * event )  
{
	if(unstableLink)
	{
		DM::Logger(DM::Warning) << "deleting lost link";
		delete unstableLink;
	}

	unstableLink = new LinkNode();

	if(portType == DM::INPORT)
	{
		unstableLink->setInPort(this);
		unstableLink->setOutPort(event->scenePos());
	}
	else
	{
		unstableLink->setOutPort(this);
		unstableLink->setInPort(event->scenePos());
	}

	this->scene()->addItem(unstableLink);
}

void PortNode::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event ) 
{
	if(!unstableLink)
		return;

	QList<QGraphicsItem*> items = this->scene()->items(event->scenePos());
	foreach (QGraphicsItem  * item, items)
	{
        if(PortNode* port = dynamic_cast<PortNode*>(item))
		{
			if((this->getType() == DM::OUTPORT && port->getType() == DM::INPORT)
				|| (this->getType() == DM::INPORT && port->getType() == DM::INPORT && this->getModule()->isGroup())
				|| (this->getType() == DM::OUTPORT && port->getType() == DM::OUTPORT && port->getModule()->isGroup()))
				simulation->addLink(this->module,	this->getPortName().toStdString(),
					port->module, port->getPortName().toStdString());

			else if((this->getType() == DM::INPORT && port->getType() == DM::OUTPORT)
				|| (this->getType() == DM::INPORT && port->getType() == DM::INPORT && port->getModule()->isGroup())
				|| (this->getType() == DM::OUTPORT && port->getType() == DM::OUTPORT && this->getModule()->isGroup()))
				// swapped
				simulation->addLink(port->module,	port->getPortName().toStdString(),
					this->module, this->getPortName().toStdString());

			break;
		}
	}
	if(unstableLink)
	{
		//this->scene()->removeItem(unstableLink);	fixes a bug when creating 2 free links
		delete unstableLink;
		unstableLink = NULL;
	}
}
