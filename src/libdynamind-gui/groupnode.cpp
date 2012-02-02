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
#include "groupnode.h"
#include <linknode.h>
#include <moduledescription.h>
#include <QGraphicsDropShadowEffect>
#include <modelnodebutton.h>
#include <ColorPalette.h>

#include <group.h>
#include <module.h>
#include <simulation.h>
#include <guiport.h>
#include <porttuple.h>
#include <boost/foreach.hpp>
#include <group.h>
#include <iostream>

using namespace boost;

GroupNode::GroupNode()
{

}
GroupNode::~GroupNode() {

    while (this->childnodes.size() > 0)
        delete this->childnodes[0];

    this->OutputTuplePorts.clear();
    this->InPortTuplePorts.clear();


}


void GroupNode::removeModelNode(ModelNode *m) {
    int index = childnodes.indexOf(m);
    this->childnodes.remove(index);
}

void GroupNode::changeGroupID(QString Name) {
    QVector<ModelNode*> ms;// =this->groups->getModelNodes(this->getName() );
    foreach (ModelNode * m, ms) {
        //std::cout << m->getGroupID().toStdString() << std::endl;
        //m->setGroupID(Name);
    }
    std::cout << "Change" << std::endl;
    std::cout << this->getName().toStdString() << std::endl;
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

void GroupNode::updatePorts () {
    DM::Group * g = (DM::Group*)this->VIBeModule;

    BOOST_FOREACH (DM::PortTuple * p,g->getInPortTuples()){
        this->addTuplePort(p);

    }
    BOOST_FOREACH (DM::PortTuple * p,g->getOutPortTuples()){
        this->addTuplePort(p);

    }
    ModelNode::updatePorts();
}

void GroupNode::addTuplePort(DM::PortTuple * p) {
    QStringList ExistingPorts;

    if (p->getPortType() == DM::INTUPLEDOUBLEDATA)
        ExistingPorts = this->ExistingInPorts;
    if (p->getPortType() == DM::OUTTUPLEDOUBLEDATA)
        ExistingPorts = this->ExistingOutPorts;
    foreach (QString pname, ExistingPorts) {
        if (pname.compare(QString::fromStdString(p->getName())) == 0) {
            return;
        }
    }
    if  (p->getPortType() > DM::OUTPORTS) {
        GUIPortTuple * gui_pt = new GUIPortTuple();
        ExistingInPorts << QString::fromStdString(p->getName());
        GUIPort * gui_p = new  GUIPort(this, p->getInPort());
        gui_pt->inPort = gui_p;
        this->ports.append(gui_p);
        gui_p->setPos(-gui_p->boundingRect().width(),gui_p->boundingRect().height()*this->inputCounter);

        gui_p = new  GUIPort(this, p->getOutPort());
        gui_pt->outPort = gui_p;
        this->ports.append(gui_p);
        gui_p->setPos(0,gui_p->boundingRect().height()*this->inputCounter++);
        this->InPortTuplePorts.append(gui_pt);
    } else {
        ExistingOutPorts << QString::fromStdString(p->getName());
        GUIPortTuple * gui_pt = new GUIPortTuple();

        GUIPort * gui_p = new  GUIPort(this, p->getInPort());
        this->ports.append(gui_p);
        gui_pt->inPort = gui_p;
        gui_p->setPos( this->boundingRect().width()-gui_p->boundingRect().width(),gui_p->boundingRect().height()*this->outputCounter);

        gui_p = new  GUIPort(this, p->getOutPort());
        gui_pt->outPort = gui_p;
        this->ports.append(gui_p);
        gui_p->setPos(  this->boundingRect().width(),gui_p->boundingRect().height()*this->outputCounter++);
        this->OutputTuplePorts.append(gui_pt);
    }


}
GUIPort *  GroupNode::getGUIPort(DM::Port * p) {
    foreach(GUIPortTuple * gui_pt,this->OutputTuplePorts) {
        if (gui_pt->inPort->getVIBePort() == p)
            return gui_pt->inPort;
        if (gui_pt->outPort->getVIBePort() == p)
            return gui_pt->outPort;
    }
    foreach(GUIPortTuple * gui_pt,this->InPortTuplePorts) {
        if (gui_pt->inPort->getVIBePort() == p)
            return gui_pt->inPort;
        if (gui_pt->outPort->getVIBePort() == p)
            return gui_pt->outPort;
    }



    return ModelNode::getGUIPort( p);
    std::cout << "NO PORT FOUND" << std::endl;
    return 0;
}

void GroupNode::removeTuplePort(int Type, QString s) {

}

GroupNode::GroupNode(  DM::Module *module, GUISimulation * s): ModelNode( module, s)
{

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


    this->outputCounter = 1;
    this->inputCounter = 1;
    this->setZValue(-1);
    this->setGraphicsEffect(new  QGraphicsDropShadowEffect(this));
    //this->setVisible(false);

    this->setFlag(QGraphicsItem::ItemIsSelectable, true);
    this->setFlag(QGraphicsItem::ItemIsMovable, true);
    this->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    this->simpleTextItem = new QGraphicsSimpleTextItem (QString::number(id));
    double w = this->simpleTextItem->boundingRect().width()+40;
    w = w < 140 ? 140 : w;
    l = w+4;
    h =  this->simpleTextItem->boundingRect().height()+65;
    unordered_map<std::string, int> parameter;
    std::cout << "L "<< l << std::endl;
    std::cout << "H"  << h << std::endl;

    //QStringList list = module.parameter.keys();
    QStringList list;
    /*foreach (QString s, list)
        parameter[s.toStdString()] = module.parameter[s];*/

    for (unordered_map<std::string, int>::const_iterator it = parameter.begin(); it != parameter.end(); ++it) {
        QString name = QString::fromStdString(it->first);
        int type = it->second;

        if (type == DM::DOUBLEDATA_IN) {
            //module.inputDouble.append(name);
        }
        if (type == DM::DOUBLEDATA_OUT) {
            //module.outputDouble.append(name);
        }

    }


    minimizeButton = new ModelNodeButton(this);
    minimizeButton->moveBy(w-16, 4 );
    minimizeButton->setVisible(true);

    Color = COLOR_MODULE;
    connect( minimizeButton, SIGNAL( Maximize() ), this, SLOT( maximize() ), Qt::DirectConnection );
    connect( minimizeButton, SIGNAL( Minimize() ), this, SLOT( minimize() ), Qt::DirectConnection );

}

void GroupNode::RePosTuplePorts() {
    foreach(GUIPortTuple * pt, this->OutputTuplePorts) {
        GUIPort * gui_p = pt->inPort;

        gui_p->setPos(l-gui_p->boundingRect().width(), gui_p->pos().y());
        gui_p = pt->outPort;
        gui_p->setPos( l, gui_p->pos().y());

    }
    minimizeButton->setPos(l-18, 4);

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

void GroupNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {


    std::cout << l << "/" << h << std::endl;
    if (this->visible) {
        recalculateLandH();
        minimizeButton->setVisible(true);
        if(this->isSelected()== true) {
            painter->setBrush(Qt::gray);

        } else {
            painter->setBrush(Qt::white);
        }
        this->simpleTextItem->setText("Name:"+ QString::fromStdString(this->VIBeModule->getName())+" " +QString::number(this->zValue()));
        if (simpleTextItem->boundingRect().width()+40 > l)
                l = simpleTextItem->boundingRect().width()+40;
        painter->drawRect(0, 0, l,h);
        if (this->childnodes.size() > 0)
            this->setPos(x1-40, y1-20);

        painter->drawText(QPoint(5,15), "Name:"+ QString::fromStdString(this->VIBeModule->getName())+" " +QString::number(this->zValue()));


        if((RePosFlag) != 0) {
            RePosTuplePorts();
            RePosFlag = false;
        }

    } else {
        minimizeButton->setVisible(false);
    }

}
QRectF GroupNode::boundingRect() const {
    return QRect(0, 0, l, h);

}
void GroupNode::addModelNode(ModelNode *m) {
    //this->childnodes.push_back(m);
    //m->getVIBeModel()->setGroup((DM::Group *)this->getVIBeModel());
}





void GroupNode::recalculateLandH() {
    float lold = l;


    this->setFlag(QGraphicsItem::ItemIsMovable, true);


    if (this->childnodes.size() > 0) {
        x1 = childnodes[0]->sceneBoundingRect().x();
        y1 = childnodes[0]->sceneBoundingRect().y();
        x2 = childnodes[0]->sceneBoundingRect().x() + childnodes[0]->sceneBoundingRect().width();
        y2 = childnodes[0]->sceneBoundingRect().y() + childnodes[0]->sceneBoundingRect().height();
        for(int i = 0; i < childnodes.size(); i++) {
            ModelNode * m = childnodes.at(i);
            if (m->sceneBoundingRect().x() < x1)
                x1 = m->sceneBoundingRect().x();
            if (m->sceneBoundingRect().y() < y1)
                y1 = m->sceneBoundingRect().y();
            if (m->sceneBoundingRect().x()+m->sceneBoundingRect().width() > x2)
                x2 = m->sceneBoundingRect().x()+m->sceneBoundingRect().width();
            if (m->sceneBoundingRect().y()+m->sceneBoundingRect().height() > y2)
                y2 = m->sceneBoundingRect().y()+m->sceneBoundingRect().height();
        }
    }

    if(!this->minimized) {
        l = x2-x1+80;
        h =  y2-y1+40;
    } else {
        l = 100;
        h = 85;
    }
    if (l > 500 || h > 500) {
        std::cout << "errer!" << std::endl;
    }
    if((lold - l) != 0) {
        RePosFlag = true;
        this->prepareGeometryChange();
        this->update();
    }


}
