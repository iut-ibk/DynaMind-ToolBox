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
#include "guiport.h"
#include <QGraphicsDropShadowEffect>

#include <guisimulation.h>

/*#include <linknode.h>
#include <moduledescription.h>
#include <modelnodebutton.h>
#include <ColorPalette.h>

#include <dmgroup.h>
#include <dmmodule.h>
#include <dmsimulation.h>
#include <guiport.h>
#include <dmporttuple.h>
#include <dmgroup.h>
#include <iostream>
#include <rootgroupnode.h>

GroupNode::GroupNode()
{

}
GroupNode::~GroupNode() {


    this->OutputTuplePorts.clear();
    this->InPortTuplePorts.clear();


    DM::Logger(DM::Debug) << "Remove GroupNode " << this->UUID;

    emit removeGroupNode(QString::fromStdString(this->UUID));

}


void GroupNode::removeModelNode(ModelNode *m) {
    int index = childnodes.indexOf(m);
    this->childnodes.remove(index);
}

void GroupNode::changeGroupID(QString Name) {
    this->getName() = Name;
}
void GroupNode::minimize() {
    this->minimized = true;
    foreach(ModelNode * m, this->childnodes) {
        if (m->isGroup()) {
            GroupNode * g = (GroupNode*) m;
            g->setMinimized( true );
            g->minimize();
        } else {
            m->setMinimized( true );
            m->update();
        }
    }
    recalculateLandH();
    if(this->scene() != 0)
        this->scene()->update();

}


void GroupNode::maximize() {
    this->minimized = false;
    foreach(ModelNode * m, this->childnodes) {
        if (m->isGroup()) {
            GroupNode * g = (GroupNode*) m;
            //g->maximize();
            g->setMinimized (false);
        } else {
            m->setMinimized (false);
            m->update();
        }
    }
    recalculateLandH();
    if(this->scene() != 0)
        this->scene()->update();


}
*/
/*
void GroupNode::updatePorts() 
 {
    DM::Group * g = (DM::Group*)this->getDMModel();

    foreach (DM::PortTuple * p,g->getInPortTuples()){
        this->addTuplePort(p);

    }
    foreach (DM::PortTuple * p,g->getOutPortTuples()){
        this->addTuplePort(p);

    }*/
    //ModelNode::updatePorts();

	/*foreach(PortNode* p, ports)
	{
		bool isNew = true;
		foreach(PortNode* gp, groupPorts)
			if(p->getPortName() == gp->getPortName())
				isNew = false;

		if(isNew)
		{
			groupPorts.push_back(new PortNode(p->getPortName(), this, p->getType()));
		}
	}
}*/
/*
void GroupNode::addTuplePort(DM::PortTuple * p) {

    //Inport
    if  (p->getPortType() > DM::OUTPORTS) {
        foreach (QString pname, ExistingInPorts) {
            if (pname.compare(QString::fromStdString(p->getName())) == 0) {
                return;
            }
        }
        ExistingInPorts << QString::fromStdString(p->getName());
        PortNode * gui_p = new  PortNode(this, p->getInPort());
        this->ports.append(gui_p);
        gui_p->setPos(0,gui_p->boundingRect().height()*this->inputCounter++);

    } else {//Outport
        foreach (QString pname, ExistingOutPorts) {
            if (pname.compare(QString::fromStdString(p->getName())) == 0) {
                return;
            }
        }
        ExistingOutPorts << QString::fromStdString(p->getName());

        PortNode * gui_p = new  PortNode(this,p->getOutPort());
        this->ports.append(gui_p);
        gui_p->setPos( this->boundingRect().width(),gui_p->boundingRect().height()*this->outputCounter++);

    }


}
PortNode *  GroupNode::getGUIPort(DM::Port * p) {
    foreach(GUIPortTuple * gui_pt,this->OutputTuplePorts) {
        if (gui_pt->inPort->getVIBePort() == p)
            return gui_pt->inPort;
        if (gui_pt->outPort->getVIBePort() == p)
            return gui_pt->outPort;
    }
    foreach(GUIPortTuple * gui_pt,this->InPortTuplePorts) {
        if (gui_pt->inPort->getVIBePort() == p)
            return gui_pt->inPort;
    }



    return ModelNode::getGUIPort( p);

    return 0;
}

void GroupNode::removeTuplePort(int Type, QString s) {

}
*/
/*
void GroupNode::notifyAddPort(const std::string &name, const DM::PortType type)
{
	groupPorts.append(new PortNode(name, 
	addPort(name, type);
}
void GroupNode::notifyRemovePort(const std::string &name, const DM::PortType type)
{
	removePort(name, type);
}*/

GroupNode::GroupNode(DM::Module *module, GUISimulation* s, SimulationTab* tab, ModelNode* parent)//: DM::ModuleObserver(m)
:ModelNode(module, s), owningTab(tab), parent(parent)
{
	/*
    this->childnodes = QVector<ModelNode*>();
    this->minimized = false;
    this->visible = true;
    this->setParentItem(0);
    this->outputCounter = 0;
    this->inputCounter = 0;
    this->x1 = 0;
    this->y1 = 0;
    this->x2 = 0;
    this->y2 = 0;
    this->UUID = module->getUuid();
    this->rg = 0;


    this->outputCounter = 1;
    this->inputCounter = 1;
    this->setZValue(-1);*/

	// while the effect is nice, the performance is bad
    //this->setGraphicsEffect(new QGraphicsDropShadowEffect(this));

    //this->setVisible(false);
	/*
    this->setFlag(QGraphicsItem::ItemIsSelectable, true);
    this->setFlag(QGraphicsItem::ItemIsMovable, true);
    this->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    this->simpleTextItem = new QGraphicsSimpleTextItem (QString::number(id));
    double w = this->simpleTextItem->boundingRect().width()+40;
    w = w < 80 ? 80 : w;
    l = w+4;
    h =  this->simpleTextItem->boundingRect().height()+65;



    Color = COLOR_MODULE;

    this->updatePorts();
	*/
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

	foreach(DM::Module* m, this->getSimulation()->getModules())
	{
		if(m->getOwner() == this->module)
		{
			if(ModelNode* mn = getSimulation()->getModelNode(m))
			{
				QPoint pos = mn->scenePos().toPoint();
				minx = min(minx, pos.x());
				miny = min(miny, pos.y());
				maxx = max(maxx, pos.x()+(int)mn->boundingRect().width());
				maxy = max(maxy, pos.y()+(int)mn->boundingRect().height());
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


/*
void GroupNode::RePosTuplePorts() {
    foreach(PortNode * gui_p, this->ports) {
        if (gui_p->getPortType()  < DM::OUTPORTS) {
            gui_p->setPos(this->boundingRect().width(), gui_p->pos().y());
        }
    }

}
void GroupNode::setSelected(  bool selected ) {
    foreach(ModelNode * m, this->childnodes) {
        m->setSelected(true);
        if (m->isGroup()) {
            GroupNode * g = (GroupNode *) m;
            g->setSelected(selected);
        }
    }

    QGraphicsItem::setSelected ( selected );


}
*/

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
    //if (this->visible) 
	{
		// shortcuts
        //CheapShadowEffect(painter, boundingRect());

        painter->setBrush(QColor(239,235,226));
        painter->setPen(QPen(Qt::black, 4.0));
		painter->drawRect(boundingRect());
		/*
        recalculateLandH();
        if(this->isSelected()== true) {
            painter->setBrush(Qt::gray);

        } else {
            painter->setBrush(Qt::white);
        }
        if (h< 65)
            h = 65;
        this->simpleTextItem->setText("Name:"+ QString::fromStdString(this->getDMModel()->getName())+" " +QString::number(this->zValue()));
        float lold = l;
        if (simpleTextItem->boundingRect().width()+40 > l)
            l = simpleTextItem->boundingRect().width()+40;
        painter->drawRect(0, 0, l,h);
        if (this->childnodes.size() > 0)
            this->setPos(x1-40, y1-20);

        painter->drawText(QPoint(5,15), "Name:"+ QString::fromStdString(this->getDMModel()->getName())+" " +QString::number(this->zValue()));

        if (lold != l)
            RePosFlag = true;
        if(RePosFlag) {
            RePosTuplePorts();
            RePosFlag = false;
        }
		*/
		this->update();
    }
	
}
/*
QRectF GroupNode::boundingRect() const {
    return QRect(-10, -10, width+10, height+10);

}*/



