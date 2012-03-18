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
#include "QBrush"
#include "iostream"
#include "guilink.h"
#include <QGraphicsSceneMouseEvent>
#include <QList>
#include "modelnode.h"
#include "ColorPalette.h"

LinkNode::LinkNode(QString LinkName, int LinkType,  ModelNode * modelNode, LinkNodeTuple * ltuple) : QGraphicsItem(modelNode)
{
    this->setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);
    this->hoverElement = 0;
    this->x1 = 0;
    this->isHover = false;
    this->tmp_link = 0;
    this->LinkType = LinkType;
    this->simpleTextItem = new QGraphicsSimpleTextItem (LinkName);
    if (LinkType == INRASTER)
        color = COLOR_RASTERPORT;
    if (LinkType == OUTRASTER)
        color = COLOR_RASTERPORT;
    if (LinkType == INVEC || LinkType == INPOINT || LinkType == INEDGE )
        color = COLOR_VECTORPORT;
    if (LinkType == OUTVEC || LinkType == OUTPOINT || LinkType == OUTEDGE )
        color = COLOR_VECTORPORT;
    if (LinkType  == INDOUBLE)
        color = COLOR_DOUBLEPORT;
    if (LinkType  == OUTDOUBLE)
        color = COLOR_DOUBLEPORT;




    this->setAcceptHoverEvents(true);
    this->setAcceptsHoverEvents(true);
    this->LinkMode = false;
    this->linked = false;
    this->modelNode = modelNode;
    this->PortName = LinkName;
    this->Tuple = false;
    this->ltuple = ltuple;
}
QString LinkNode::getPortName(bool flag) {
    if (!Tuple || flag) {
        return this->PortName;
    } //else  {
        //if (this->ltuple->Port1 != 0)
            //return this->ltuple->Port1->links[0]->getStartNode()->getPortName();
        //else return "";
    //}
}

void LinkNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {

    painter->setBrush(color);
    if(isHover){
        l = this->simpleTextItem->boundingRect().width()+4;
        h = this->simpleTextItem->boundingRect().height()+4;
        x1 = 0;
        if (LinkType == INVEC || LinkType == INPOINT || LinkType == INEDGE || LinkType == INRASTER || LinkType == INDOUBLE)
            x1 = -l+14;

        QPainterPath path;
        QPen pen(Qt::black, 1.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        path.addRoundRect(x1, 0,l,h, 10);
        painter->fillPath(path, color);
        painter->strokePath(path, pen);
        painter->setPen(pen);
        painter->drawText(x1+2,15, this->simpleTextItem->text());


    } else {
        QPainterPath path;
        QPen pen(Qt::black, 1.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        path.addRoundRect(0, 0,14,14, 10);
        painter->fillPath(path, color);
        painter->strokePath(path, pen);


        //painter->drawRect(0, 0,  14, 14);
    }

    painter->setBrush(Qt::NoBrush);
}
QRectF LinkNode::boundingRect() const {
    if(isHover){
        QRect r (x1, 0,
                 l, h);
        return r;
    } else {
        QRect r (0, 0,
                 14, 25);
        return r;
    }
}
void LinkNode::hoverEnterEvent ( QGraphicsSceneHoverEvent * event ) {

    this->isHover = true;
    if (LinkType == INRASTER)
        color = COLOR_RASTERPORT;
    if (LinkType == OUTRASTER)
        color = COLOR_RASTERPORT;
    if (LinkType == INVEC || LinkType == INPOINT || LinkType == INEDGE )
        color = COLOR_VECTORPORT;
    if (LinkType == OUTVEC || LinkType == OUTPOINT || LinkType == OUTEDGE )
        color = COLOR_VECTORPORT;
    if (LinkType  == INDOUBLE)
        color = COLOR_DOUBLEPORT;
    if (LinkType  == OUTDOUBLE)
        color = COLOR_DOUBLEPORT;
    if(LinkType == INVEC || LinkType == INPOINT || LinkType == INEDGE || LinkType == INDOUBLE || LinkType == INRASTER ) {
        if  (this->linked == true) {
            //color = Qt::white;
        }
    }
    prepareGeometryChange ();
    l = this->simpleTextItem->boundingRect().width()+4;
    h = this->simpleTextItem->boundingRect().height()+4;
    x1 = 0;
    if (LinkType == INVEC || LinkType == INPOINT || LinkType == INEDGE || LinkType == INRASTER || LinkType == INDOUBLE)
        x1 = -l+14;
    this->update(this->boundingRect());
}

void LinkNode::hoverLeaveEvent ( QGraphicsSceneHoverEvent * event ) {
    this->isHover = false;
    if (!LinkMode) {
        if (LinkType == INRASTER)
            color = COLOR_RASTERPORT;
        if (LinkType == OUTRASTER)
            color = COLOR_RASTERPORT;
        if (LinkType == INVEC || LinkType == INPOINT || LinkType == INEDGE )
            color = COLOR_VECTORPORT;
        if (LinkType == OUTVEC || LinkType == OUTPOINT || LinkType == OUTEDGE )
            color = COLOR_VECTORPORT;
        if (LinkType  == INDOUBLE)
            color = COLOR_DOUBLEPORT;
        if (LinkType  == OUTDOUBLE)
            color = COLOR_DOUBLEPORT;
    }
    if(LinkType == INVEC || LinkType == INPOINT || LinkType == INEDGE || LinkType == INDOUBLE || LinkType == INRASTER) {
        if  (this->linked == true) {
            //color = Qt::white;
        }
    }

    prepareGeometryChange ();
    this->update(this->boundingRect());
}

LinkNode::~LinkNode() {
    QList<GUILink * > links_tmp = links;
    this->links.clear();
    delete this->simpleTextItem;

    foreach (GUILink * l, links_tmp) {
        delete l;
        l=0;
    }

}

void LinkNode::removeGUILink(GUILink * l) {
    int index = this->links.indexOf(l);
    this->links.removeAt(index);
    if (this->links.size() == 0) {
        this->linked = false;

    }
}
void  LinkNode::setVisible(bool visible) {
    if (this->getLinked()) {
        foreach ( GUILink * j, links) {
            j->setVisible(visible);
        }
    }
    QGraphicsItem::setVisible(visible);
}

void LinkNode::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )  {
    this->scene()->sendEvent(0, event);
    //if (LinkMode) {
        //this->tmp_link->setEndNode(event->scenePos());
    //}
    QList<QGraphicsItem  *> items = this->scene()->items(event->scenePos());
    //Check Hover Event
    bool setHover = false;
    foreach (QGraphicsItem  * item, items) {
        if ( this->type() == item->type() ) {
            LinkNode * link  = (LinkNode *) item;
            if (getLinkType() == OUTRASTER &&  link->getLinkType() == INRASTER ||
                getLinkType() == OUTVEC &&  link->getLinkType() == INVEC ||
                getLinkType() == OUTPOINT &&  link->getLinkType() == INPOINT ||
                getLinkType() == OUTEDGE &&  link->getLinkType() == INEDGE ||
                getLinkType() == OUTDOUBLE &&  link->getLinkType() == INDOUBLE
                ) {
                if ( link->getLinked() == false) {
                    link->setHover(true);
                    link->prepareGeometryChange();
                    link->update();
                    this->hoverElement = link;
                    setHover = true;
                }
            }
        }
    }
    if(!setHover) {
        if(this->hoverElement != 0) {
            this->hoverElement->setHover(false);
            this->hoverElement->prepareGeometryChange();
            this->hoverElement->update();
        }
        this->hoverElement = 0;
    }

}

void LinkNode::mousePressEvent ( QGraphicsSceneMouseEvent * event )  {
    if (LinkType == INRASTER)
        color = COLOR_RASTERPORT;
    if (LinkType == OUTRASTER)
        color = COLOR_RASTERPORT;
    if (LinkType == INVEC || LinkType == INPOINT || LinkType == INEDGE )
        color = COLOR_VECTORPORT;
    if (LinkType == OUTVEC || LinkType == OUTPOINT || LinkType == OUTEDGE )
        color = COLOR_VECTORPORT;
    if (LinkType  == INDOUBLE)
        color = COLOR_DOUBLEPORT;
    if (LinkType  == OUTDOUBLE)
        color = COLOR_DOUBLEPORT;

    if (LinkType == OUTRASTER || LinkType ==  OUTVEC || LinkType ==  OUTDOUBLE) {
        LinkMode = true;
        this->tmp_link = new GUILink();
        //this->tmp_link->setStartNode(this);
        this->scene()->addItem(this->tmp_link);
    }



}
void  LinkNode::refreshLinks() {
    foreach(GUILink * l, this->links) {
        if ( l != 0) {
            l->refresh();
        }
    }
}

QVariant LinkNode::itemChange(GraphicsItemChange change, const QVariant &value) {
    if(change == QGraphicsItem::ItemScenePositionHasChanged) {
        this->refreshLinks();
    }
    return QGraphicsItem::itemChange(change, value);
}

void LinkNode::mouseReleaseEvent ( QGraphicsSceneMouseEvent * event ) {
    std::cout << "mouseReleaseEvent " << std::endl;
    if (this->LinkType == OUTRASTER || LinkType ==  OUTVEC || LinkType == OUTPOINT || LinkType == OUTEDGE || LinkType ==  OUTDOUBLE) {
        LinkMode = false;
        if (LinkType == INRASTER)
            color = COLOR_RASTERPORT;
        if (LinkType == OUTRASTER)
            color = COLOR_RASTERPORT;
        if (LinkType == INVEC || LinkType == INPOINT || LinkType == INEDGE )
            color = COLOR_VECTORPORT;
        if (LinkType == OUTVEC || LinkType == OUTPOINT || LinkType == OUTEDGE )
            color = COLOR_VECTORPORT;
        if (LinkType  == INDOUBLE)
            color = COLOR_DOUBLEPORT
                    if (LinkType  == OUTDOUBLE)
                        color = COLOR_DOUBLEPORT;

            this->update(this->boundingRect());
            QList<QGraphicsItem  *> items = this->scene()->items(event->scenePos());
            bool newLink = false;
            foreach (QGraphicsItem  * item, items) {
                if ( this->type() == item->type() ) {
                    LinkNode * endLink  = (LinkNode *) item;
                    if (getLinkType() == OUTRASTER &&  endLink->getLinkType() == INRASTER ||
                        getLinkType() == OUTVEC &&  endLink->getLinkType() == INVEC ||
                        getLinkType() == OUTPOINT &&  endLink->getLinkType() == INPOINT ||
                        getLinkType() == OUTEDGE &&  endLink->getLinkType() == INEDGE ||
                        getLinkType() == OUTDOUBLE&&  endLink->getLinkType() == INDOUBLE
                        ) {
                        if ( endLink->getLinked() == false) {
                            //this->tmp_link->setEndNode(endLink);
                            this->links.append(tmp_link);
                            this->linked = true;
                            newLink = true;
                            tmp_link = 0;
                        }
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
QPointF LinkNode::getConnectionNode() {

    return  QPointF(this->scenePos());
}
void LinkNode::setLink(GUILink * l) {
    this->links.append(l);
    this->linked = true;

}


