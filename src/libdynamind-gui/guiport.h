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
#ifndef GUIPORT_H
#define GUIPORT_H

#include "dmcompilersettings.h"
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QString>
#include <QGraphicsSimpleTextItem>
#include <QPainter>
#include <QColor>
#include <QObject>
#include "dmcompilersettings.h"
/*
namespace DM {
class Port;
}*/

class ModelNode;
class GUILink;
class GUISimulation;

class DM_HELPER_DLL_EXPORT GUIPort : public QGraphicsItem
{
private:


    QColor color;
    ModelNode * modelNode;
    GUILink * tmp_link;
    QVector<GUILink * > links;
    QString PortName;
    int PortType;
    bool isHover;
    bool LinkMode;
    GUIPort * hoverElement;
    float l;
    float h;
    float x1;
     QGraphicsSimpleTextItem portname_graphics;
     GUISimulation * simulation;

public:
    GUIPort(ModelNode * modelNode/*, DM::Port * p*/);
    ~GUIPort();
    void hoverEnterEvent ( QGraphicsSceneHoverEvent * event );
    void hoverLeaveEvent ( QGraphicsSceneHoverEvent * event );
    void mouseMoveEvent ( QGraphicsSceneMouseEvent * event );
    void mousePressEvent ( QGraphicsSceneMouseEvent * event );
    void mouseReleaseEvent ( QGraphicsSceneMouseEvent * event );
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    void setLink(GUILink * l);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    QRectF boundingRect() const;
    QPointF getConnectionNode();
    int getPortType();
    QString getPortName(){return this->PortName;}
    bool isLinked();
    void setHover(bool b){this->isHover=b;}
    void refreshLinks();
    //DM::Port * getVIBePort();
    void removeLink(GUILink * l);
    //void updatePort(DM::Port * p);
    GUISimulation * getSimulation() {return this->simulation;}
    void  setSimulation(GUISimulation *s) {this->simulation = s;}



};

#endif // GUIPORT_H
