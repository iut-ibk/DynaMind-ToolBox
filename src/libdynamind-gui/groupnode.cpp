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

#include "groupnode.h"
#include "portnode.h"
#include <QGraphicsDropShadowEffect>

#include <guisimulation.h>

GroupNode::GroupNode(DM::Module *module, GUISimulation* s, SimulationTab* tab, ModelNode* parent)
	:ModelNode(module, s), owningTab(tab), parent(parent)
{
	this->setFlag(QGraphicsItem::ItemIsSelectable, false);
	this->setFlag(QGraphicsItem::ItemIsMovable, false);

	resize();
}

void GroupNode::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) 
{
}

void GroupNode::resize()
{
	const int border = 30;
	int minx = 0;
	int maxx = 200;
	int miny = 0;
	int maxy = 200;
	bool first = true;
	foreach(DM::Module* m, this->getSimulation()->getModules())
	{
		if(m->getOwner() == this->module)
		{
			if(ModelNode* mn = getSimulation()->getModelNode(m))
			{
				QPoint pos = mn->scenePos().toPoint();
				if(first) 
				{
					minx = pos.x();
					miny = pos.y();
					maxx = pos.x()+(int)mn->boundingRect().width();
					maxy = pos.y()+(int)mn->boundingRect().height();
					first = false;
				}
				else
				{
					minx = min(minx, pos.x());
					miny = min(miny, pos.y());
					maxx = max(maxx, pos.x()+(int)mn->boundingRect().width());
					maxy = max(maxy, pos.y()+(int)mn->boundingRect().height());
				}
			}
		}
	}
	width = maxx-minx + 2*border;
	height = maxy-miny + 2*border;
	setPos(QPointF(minx - border, miny - border));

	this->update();

	foreach(PortNode* pn, this->ports)
		pn->updatePos();
}

GroupNode::~GroupNode()
{
	simulation->closeTab(owningTab);	
}

void CheapShadowEffect(QPainter *painter, const QRectF boundingRect)
{
	float x = boundingRect.x();
	float y = boundingRect.y();
	float w = boundingRect.width();
	float h = boundingRect.height();
	float b = 20;

	// setup the bottom shadow poly
	QPolygon poly_bottom;
	poly_bottom.append(QPoint(x,		y+h));
	poly_bottom.append(QPoint(x+w,	y+h));
	poly_bottom.append(QPoint(x+w+b,	y+h+b));
	poly_bottom.append(QPoint(x+b,		y+h+b));

	QLinearGradient grad_bottom(0, y+w+b, 0, y+w);
	grad_bottom.setColorAt(0, Qt::white);
	grad_bottom.setColorAt(1, Qt::gray);
	painter->setBrush(grad_bottom);
	painter->setPen(QPen(Qt::white, 0.0, Qt::NoPen));

	painter->drawPolygon(poly_bottom);

	// setup the right shadow poly
	QPolygon poly_right;
	poly_right.append(QPoint(x+w,	y));
	poly_right.append(QPoint(x+w+b,	y+b));
	poly_right.append(QPoint(x+w+b,	y+h+b));
	poly_right.append(QPoint(x+w,	y+h));

	QLinearGradient grad_right(x+w+b,0,x+w,0);
	grad_right.setColorAt(0, Qt::white);
	grad_right.setColorAt(1, Qt::gray);
	painter->setBrush(grad_right);
	painter->setPen(QPen(Qt::white, 0.0));

	painter->drawPolygon(poly_right);
}

void GroupNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	painter->setBrush(QColor(239,235,226));
	painter->setPen(QPen(Qt::black, 4.0));
	painter->drawRect(boundingRect());
}


