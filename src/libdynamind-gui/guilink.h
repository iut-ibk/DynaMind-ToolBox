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
#ifndef GUILINK_H
#define GUILINK_H

#include "dmcompilersettings.h"
#include <QGraphicsItem>
#include <QLineF>
#include <QPainterPath>
#include <dmsimulation.h>
#include <algorithm>

#include "dmcompilersettings.h"
class PortNode;
class GUISimulation;

class  DM_HELPER_DLL_EXPORT GUILink : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
    //enum { Type = UserType + 3 };
private:
    QLineF line;
    PortNode * inPort;
    PortNode * outPort;
    void updatePaths();
    QPainterPath connection_path, handle_path, united;
    QPointF source, sink;
    bool hovered;
    GUISimulation * sim;
protected:
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
public:
    GUILink();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    QRectF boundingRect() const;
    void setOutPort(PortNode * outPort);
    void setOutPort(QPointF p);
    void setInPort(PortNode * inPort);
    void setInPort(QPointF p);
    int type() const {return Type; }
    void refresh();
    PortNode * getOutPort(){return outPort;}
    PortNode * getInPort(){return inPort;}
    void setSimulation(GUISimulation * sim) {this->sim = sim;}
    ~GUILink();
public slots:
    void deleteLink();

};

#endif // GUILINK_H
