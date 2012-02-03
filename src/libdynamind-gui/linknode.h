/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of VIBe2
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
#ifndef LINKNODE_H
#define LINKNODE_H

#include "compilersettings.h"
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QString>
#include <QGraphicsSimpleTextItem>
#include <QPainter>
#include <QColor>
#include <QObject>

enum LinkTypes {
    OUTRASTER = 1,
    INRASTER,
    OUTVEC,
    OUTPOINT,
    OUTEDGE,
    INVEC,
    INPOINT,
    INEDGE,
    INPUTTUPLERASTER,
    OUTPUTTUPLERASTER,
    INPUTTUPLEVECTOR,
    OUTPUTTUPLEVECTOR,
    INPUTTUPLEDOUBLE,
    OUTPUTTUPLEDOUBLE,
    INDOUBLE,
    OUTDOUBLE

};

//Have the Output 1
//Have the Input 2
class GUILink;
class ModelNode;
class LinkNode;

struct DM_HELPER_DLL_EXPORT LinkNodeTuple {
    LinkNode * Port1;
    LinkNode * Port2;
};

class  DM_HELPER_DLL_EXPORT LinkNode :  public QGraphicsItem
{
    enum { Type = UserType + 2 };
private:
    QGraphicsSimpleTextItem * simpleTextItem;
    QColor color;
    int LinkType;
    bool LinkMode;
    bool Tuple;
    GUILink * tmp_link;
    QList<GUILink * > links;
    bool linked;
    ModelNode * modelNode;
    QString PortName;
    bool isHover;
    LinkNode * hoverElement;
    float l;
    float h;
    float x1;
    LinkNodeTuple * ltuple;
public:
    LinkNode(QString LinkName, int LinkType, ModelNode * modelNode,LinkNodeTuple * ltuple = 0);
    void hoverEnterEvent ( QGraphicsSceneHoverEvent * event );
    void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event );
    void mouseMoveEvent ( QGraphicsSceneMouseEvent * event ) ;
    void mousePressEvent ( QGraphicsSceneMouseEvent * event ) ;
    void mouseReleaseEvent ( QGraphicsSceneMouseEvent * event );

    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    void setVisible(bool visible);
    ~LinkNode();
    void removeGUILink(GUILink * l);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    QRectF boundingRect() const;
    QPointF getConnectionNode();
    int type() const {return Type; }
    int getLinkType() {return LinkType;}
    bool getLinked(){return linked;}
    void setLink(GUILink * l);
    QList<GUILink * > getLinks(){return this->links;}
    ModelNode * getModelNode(){return this->modelNode;}
    QString getPortName(bool flag = true);

    void RemoveGUILink(GUILink * l);
    void setHover(bool b){this->isHover=b;}
    bool isTuple(){return this->Tuple;}
    void setIsTuple(bool b){this->Tuple = b;}
    LinkNodeTuple * getLinkTuple(){return this->ltuple;}

    void refreshLinks();


};

#endif // LINKNODE_H
