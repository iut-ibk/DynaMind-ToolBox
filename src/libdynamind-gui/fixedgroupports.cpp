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

#include "fixedgroupports.h"
#include "QPen"
#include "QPainter"
#include "QGraphicsScene"
#include "QGraphicsView"
#include "QRect"
#include "QRectF"
#include "QPen"
FixedGroupPorts::FixedGroupPorts() {
    this->setFlag(QGraphicsItem::ItemIgnoresTransformations);
}

void FixedGroupPorts::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {


    QPainterPath path;
    QPen pen(Qt::black, 1.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    QGraphicsView * view =this->scene()->views()[0];
    QRectF scenerec1 = this->scene()->sceneRect();
    QRectF scenerec = view->sceneRect();

    QPoint v = view->viewport()->rect().center();

    QRect p (v.x(),v.y(),10,10);

    QRectF p1 = view->mapToScene(v.x(),v.y(),10,10).boundingRect();



    path.addEllipse(p1);
    painter->fillPath(path, Qt::white);
    painter->strokePath(path, pen);



    painter->setBrush(Qt::NoBrush);
}
QRectF FixedGroupPorts::boundingRect() const{
    QRect r (0, 0,
             0, 0);
    return r;
}
