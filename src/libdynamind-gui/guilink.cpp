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
#include "guilink.h"
#include <iostream>
#include <cmath>
#include <QMenu>
#include <QAction>
#include <QGraphicsSceneContextMenuEvent>
#include <guiport.h>
#include <guisimulation.h>
#include <dmport.h>
GUILink::GUILink(){

    inPort = 0;
    outPort = 0;
    hovered = false;
    VIBelink = 0;
    setAcceptHoverEvents(true);
    this->setZValue(100);
}

void GUILink::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    QColor c;
    c = isSelected() ? Qt::gray : Qt::white;
    if (hovered) {
        c = Qt::green;
    }
    QBrush brush(c);

    QPen pen(Qt::black);
    if (this->inPort != 0) {
        if (this->isBack())
            pen = QPen(Qt::red);
    }
    painter->strokePath(connection_path, pen);
    painter->fillPath(handle_path, brush);
    painter->strokePath(handle_path, pen);
}
QRectF GUILink::boundingRect() const {
    return united.boundingRect();
}

GUILink::~GUILink() {
    DM::Logger(DM::Debug) << "Remove GUILink" ;
    if (this->outPort != 0)

        this->outPort->removeLink(this);
    if (this->inPort != 0)
        this->inPort->removeLink(this);

    if (VIBelink != 0)
        delete this->VIBelink;

    this->VIBelink = 0;
    this->inPort = 0;
    this->outPort = 0;
}

void GUILink::setOutPort(GUIPort * outPort)
{

    if (outPort  == 0)
        return;
    this->outPort = outPort;
    outPort->setLink(this);
    source = outPort->getConnectionNode();
    sink = outPort->getConnectionNode();
    prepareGeometryChange ();
    updatePaths();

    this->update(this->boundingRect());
}

void GUILink::setInPort(GUIPort * inPort) {
    if (inPort == 0)
        return;
    this->inPort = inPort;
    inPort->setLink(this);
    sink = inPort->getConnectionNode();
    prepareGeometryChange ();
    updatePaths();
    this->update(this->boundingRect());
}
void GUILink::setInPort(QPointF p) {
    sink = p;
    prepareGeometryChange ();
    updatePaths();
    this->update(this->boundingRect());
}
void GUILink::refresh() {
    source = outPort->getConnectionNode();
    sink = inPort->getConnectionNode();
    updatePaths();
    prepareGeometryChange ();
    this->update(this->boundingRect());

}
void GUILink::deleteLink() {

    delete this;


}
void GUILink::backLink() {
    //this->back = true;

    this->VIBelink->setBackLink(true);

}
void GUILink::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
    QMenu menu;

    QAction  * a_back = menu.addAction("back");
    QAction  * a_delete = menu.addAction("delete");


    connect( a_delete, SIGNAL( activated() ), this, SLOT( deleteLink() ), Qt::DirectConnection );
    connect( a_back, SIGNAL( activated() ), this, SLOT( backLink() ), Qt::DirectConnection );
    menu.exec(event->screenPos());

}
qreal mid(qreal start, qreal stop) {
    return qMin(start, stop) + (qAbs(start - stop) / 2.0);
}
void GUILink::updatePaths() {
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
void GUILink::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    hovered = true;
    update();
}

void GUILink::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    hovered = false;
    update();
}


