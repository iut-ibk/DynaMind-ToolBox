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
GUIPort::~GUIPort () {

    foreach(GUILink *l, this->links) {
        delete l;
        l = 0;
    }
    this->links.clear();
}
void GUIPort::removeLink(GUILink * l) {
    int index = this->links.indexOf(l);
    if (index > -1) {
        this->links.remove(index);
        DM::Logger(DM::Debug) << "Remove GUILink from" << this->getPortName() << this->links.size();
    }
}
void GUIPort::updatePort(DM::Port * p) {
    //this->p = p;

}
GUIPort::GUIPort(ModelNode *modelNode, DM::Port *p) : QGraphicsItem(modelNode)
{
    this->setParentItem(modelNode);
    this->setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);
    this->setAcceptHoverEvents(true);
    this->setAcceptsHoverEvents(true);
    this->PortName = QString::fromStdString(p->getLinkedDataName());
    tmp_link = 0;
    //this->hoverElement = 0;
    //this->p = p;
    this->x1 = 0;
    this->isHover = false;
    this->LinkMode = false;
    this->modelNode = modelNode;
    this->PortType = p->getPortType();
    this->simulation = modelNode->getSimulation();
    //this->simpleTextItem = new QGraphicsSimpleTextItem (QString::fromStdString(p->getLinkedDataName()));

    if (p->getPortType() == DM::INSYSTEM || p->getPortType() == DM::OUTSYSTEM)
        color = COLOR_VECTORPORT;

    hoverElement = 0;


}
bool GUIPort::isLinked() {
    if (this->getVIBePort()->getLinks().size() > 0)
        return true;
    return false;
}

void GUIPort::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    if (this->getVIBePort()->isFullyLinked())
        color = Qt::green;
    if (!this->getVIBePort()->isFullyLinked())
        color = Qt::red;
    painter->setBrush(color);

    if(isHover){
        QPainterPath path;
        QPen pen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        path.addEllipse(-8, 8,16,16);
        painter->fillPath(path, color);
        painter->strokePath(path, pen);



    } else {
        QPainterPath path;
        QPen pen(Qt::black, 0.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        path.addEllipse(-7, 7,14,14);
        painter->fillPath(path, color);
        painter->strokePath(path, pen);

    }
    portname_graphics.setText(this->getPortName());

    if (this->getPortType() > DM::OUTPORTS)
        painter->drawText(QPoint(10,portname_graphics.boundingRect().height()/2+10), this->getPortName());
    if (this->getPortType() < DM::OUTPORTS)
        painter->drawText(QPoint(-portname_graphics.boundingRect().width()-10,portname_graphics.boundingRect().height()/2+10), this->getPortName());
    painter->setBrush(Qt::NoBrush);
}

QRectF GUIPort::boundingRect() const {
    if(this->PortType < DM::OUTPORTS){
        QRect r (-10, 10,20+this->portname_graphics.boundingRect().width(),20);
        return r;
    } else {
        QRect r (-10-this->portname_graphics.boundingRect().width(), 10,20+this->portname_graphics.boundingRect().width(),20);
        return r;
    }
}

void GUIPort::hoverEnterEvent ( QGraphicsSceneHoverEvent * event ) {
    this->isHover = true;

    if (this->PortType  == DM::INSYSTEM||this->PortType  == DM::OUTSYSTEM)
        color = COLOR_VECTORPORT;

    prepareGeometryChange ();
    l = this->portname_graphics.boundingRect().width()+4;

    h = this->portname_graphics.boundingRect().height()+4;
    x1 = 0;
    if (this->PortType   > DM::OUTPORTS  )
        x1 = -l+14;
    this->update(this->boundingRect());
}

void GUIPort::hoverLeaveEvent ( QGraphicsSceneHoverEvent * event ) {
    this->isHover = false;
    if (!LinkMode) {

        if (PortType  == DM::INSYSTEM || PortType == DM::OUTSYSTEM)
            color = COLOR_VECTORPORT;

    }
    prepareGeometryChange ();
    this->update(this->boundingRect());
}

QPointF GUIPort::getConnectionNode() {
    QPointF p(this->scenePos());

    if (this->getPortType() > DM::OUTPORTS)
        return  QPointF( p+QPointF(-7,14));
    return  QPointF( p+QPointF(7,14));
}

int GUIPort::getPortType() {
    return this->PortType;
}

void GUIPort::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )  {
    this->scene()->sendEvent(0, event);
    if (LinkMode) {
        this->tmp_link->setInPort(event->scenePos());
    }
    QList<QGraphicsItem  *> items = this->scene()->items(event->scenePos());
    //Check Hover Event
    bool setHover = false;
    foreach (QGraphicsItem  * item, items) {
        if ( this->type() == item->type() ) {
            GUIPort * link  = (GUIPort *) item;

            if (getPortType() == DM::OUTSYSTEM &&  link->getPortType() == DM::INSYSTEM ) {
                link->setHover(true);
                link->prepareGeometryChange();
                link->update();
                this->hoverElement = link;
                setHover = true;
            }
        }
    }
    if(!setHover) {
        if(this->hoverElement != 0) {
            this->hoverElement->setHover(false);
        }
        this->hoverElement = 0;
    }

}


void GUIPort::mousePressEvent ( QGraphicsSceneMouseEvent * event )  {
    std::cout << "Mouse Press event" << std::endl;
    //If currently in link mode delete node and proceed with other stuff
    if (LinkMode ) {
        if (this->tmp_link != 0) {
            delete this->tmp_link;
            this->tmp_link = 0;
        }
        LinkMode = false;
        return;
    }


    if (getPortType() == DM::INSYSTEM || getPortType() == DM::OUTSYSTEM )
        color = COLOR_VECTORPORT;



    if (getPortType() < DM::OUTPORTS) {
        LinkMode = true;
        this->tmp_link = new GUILink();
        this->tmp_link->setOutPort(this);
        this->scene()->addItem(this->tmp_link);
    }

}
DM::Port * GUIPort::getVIBePort() {
    DM::Port * p = 0;
    if (this->PortType > DM::OUTPORTS) {
        p = this->modelNode->getDMModel()->getInPort(this->getPortName().toStdString());
    } else {
        p = this->modelNode->getDMModel()->getOutPort(this->getPortName().toStdString());
    }
   return p;
}

void GUIPort::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event ) {
    if (getPortType() < DM::OUTPORTS) {
        LinkMode = false;

        if (getPortType()  == DM::INSYSTEM)
            color = COLOR_VECTORPORT;

        this->update(this->boundingRect());
        QList<QGraphicsItem  *> items = this->scene()->items(event->scenePos());
        bool newLink = false;
        foreach (QGraphicsItem  * item, items) {
            if ( this->type() == item->type() ) {
                GUIPort * endLink  = (GUIPort *) item;

                if (getPortType() == DM::OUTSYSTEM &&  endLink->getPortType() == DM::INSYSTEM ) {
                    this->tmp_link->setInPort(endLink);
                    //this->links.append(tmp_link);
                    //Create Link
                    tmp_link->setVIBeLink(this->modelNode->getSimulation()->addLink(tmp_link->getOutPort()->getVIBePort(), tmp_link->getInPort()->getVIBePort()));
                    tmp_link->setSimulation(this->modelNode->getSimulation());
                    newLink = true;
                    tmp_link = 0;

                    //Run Simulation
                    this->modelNode->getSimulation()->updateSimulation();


                }

            }

        }
        if (!newLink ) {
            if (this->tmp_link != 0) {
                delete this->tmp_link;
                this->tmp_link = 0;
            }
        }
    }


}
void GUIPort::refreshLinks() {
    foreach(GUILink * l, this->links) {
        if ( l != 0) {
            l->refresh();
        }
    }
}
void GUIPort::setLink(GUILink * l) {

    int index = this->links.indexOf(l);
    if (index == -1) {
        this->links.append(l);
        DM::Logger(DM::Debug) << "Set Link" << this->getPortName();
    }

}
QVariant GUIPort::itemChange(GraphicsItemChange change, const QVariant &value) {
    if(change == QGraphicsItem::ItemScenePositionHasChanged) {
        this->refreshLinks();
    }
    if (change == QGraphicsItem::ItemVisibleHasChanged) {
        foreach(GUILink * l, this->links)
            l->setVisible(this->isVisible());
    }
    return QGraphicsItem::itemChange(change, value);
}




