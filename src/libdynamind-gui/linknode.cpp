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

#include "linknode.h"
#include <iostream>
#include <cmath>
#include <QMenu>
#include <QAction>
#include <qobject.h>
#include <QGraphicsSceneContextMenuEvent>
#include <dmmodule.h>
#include <portnode.h>
#include <guisimulation.h>

LinkNode::LinkNode()
{
	inPort = 0;
	outPort = 0;
	hovered = false;
	setAcceptHoverEvents(true);
	this->setZValue(4);
}

void LinkNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
	QColor c;
	if(hovered)				c = Qt::green;
	else if(isSelected())	c = Qt::gray;
	else					c = Qt::white;

	QBrush brush(c);

	/*
	if (this->inPort != 0) {
	if (this->isBack())
	pen = QPen(Qt::red);
	}*/


	QPen pen(Qt::white);
	pen.setWidth(3);
	painter->strokePath(connection_path, pen);
	painter->fillPath(handle_path, brush);
	painter->strokePath(handle_path, pen);


	pen.setColor(Qt::black);
	pen.setWidth(1.5);
	painter->strokePath(connection_path, pen);
	painter->fillPath(handle_path, brush);
	painter->strokePath(handle_path, pen);
}
QRectF LinkNode::boundingRect() const 
{
	return united.boundingRect();
}

LinkNode::~LinkNode() 
{
	//DM::Logger(DM::Debug) << "Deleting LinkNode";

	if(outPort && inPort)
	{
		outPort->getSimulation()->removeLink(outPort, inPort);
	}

	if (this->outPort)
		this->outPort->removeLink(this);
	if (this->inPort)
		this->inPort->removeLink(this);

	this->inPort = 0;
	this->outPort = 0;
}

void LinkNode::setOutPort(PortNode * outPort)
{
	this->outPort = outPort;
	refresh();
}
void LinkNode::setOutPort(QPointF p) 
{
	source = p;
	refresh();
}

void LinkNode::setInPort(PortNode * inPort) 
{
	this->inPort = inPort;
	refresh();

}
void LinkNode::setInPort(QPointF p) 
{
	sink = p;
	refresh();
}


void LinkNode::refresh() 
{
	if(outPort)	source = outPort->getCenterPos() + QPointF(9,0);
	if(inPort)	sink = inPort->getCenterPos()  - QPointF(9,0);

	updatePaths();

	prepareGeometryChange ();
	this->update(this->boundingRect());
}

void LinkNode::deleteLink() 
{
	delete this;
}

void LinkNode::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) 
{
	QMenu menu;
	QAction  * a_delete = menu.addAction("delete");
	connect( a_delete, SIGNAL( triggered() ), this, SLOT( deleteLink() ), Qt::DirectConnection );
	menu.exec(event->screenPos());
}
qreal mid(qreal start, qreal stop) 
{
	return qMin(start, stop) + (qAbs(start - stop) / 2.0);
}
void LinkNode::updatePaths() {
	connection_path = QPainterPath();
	qreal x = (source - sink).x();
	QPointF c1(source.x() - x / 2.0, source.y());
	QPointF c2(sink.x() + x / 2.0, sink.y());
	connection_path.moveTo(source);
	connection_path.cubicTo(c1, c2, sink);
	handle_path = QPainterPath();
	handle_path.addEllipse(mid(source.x(), sink.x()) - 5, mid(source.y(),sink.y()) - 5, 10, 10);
	united = handle_path.united(connection_path);
	update();
}
void LinkNode::hoverEnterEvent(QGraphicsSceneHoverEvent *event) 
{
	hovered = true;
	update();
	QGraphicsItem::hoverEnterEvent(event);
}

void LinkNode::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) 
{
	hovered = false;
	update();
	QGraphicsItem::hoverLeaveEvent(event);
}


