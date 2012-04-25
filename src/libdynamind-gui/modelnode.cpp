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
#include <modelnode.h>
#include <iostream>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSimpleTextItem>
#include <linknode.h>
#include <guilink.h>
#include <guimodelnode.h>
#include <QMenu>

#include "projectviewer.h"
#include <QGraphicsDropShadowEffect>
#include <QPen>
#include <QLinearGradient>
#include "ColorPalette.h"
#include "modelnodebutton.h"

#include <dmmodule.h>
#include <guiport.h>
#include <boost/foreach.hpp>
#include <dmsimulation.h>
#include <QApplication>
#include <QInputDialog>
#include <rootgroupnode.h>
#include <guisimulation.h>
#include <dmcomponent.h>
#include <dmsystem.h>
#include <sstream>
using namespace boost;
std::string ModelNode::getParameterAsString(std::string name) {

    std::ostringstream val;
    int id = this->VIBeModule->getParameterList()[name];
    if (id == DM::DOUBLE || id == DM::LONG || id == DM::STRING) {
        if (id == DM::DOUBLE)
            val << this->VIBeModule->getParameter<double>(name);
        if (id == DM::LONG)
            val << this->VIBeModule->getParameter<long>(name);
        if (id == DM::STRING)
            val << this->VIBeModule->getParameter<std::string>(name);
        return val.str();
    }
}

std::string ModelNode::getGroupUUID() {
    return this->VIBeModule->getGroup()->getUuid();
}

void ModelNode::updatePorts () {

    //Add Ports
    //If Port exists is checked by addPort
    BOOST_FOREACH (DM::Port * p, this->VIBeModule->getInPorts()){
        this->addPort(p);
    }
    BOOST_FOREACH (DM::Port * p, this->VIBeModule->getOutPorts()) {
        this->addPort(p);
    }

    for (int i = this->ports.size()-1; i > -1; i--) {
        GUIPort * gp = this->ports[i];
        if (gp->getVIBePort()->isPortTuple())
            continue;
        if (gp->getPortType()  > DM::OUTPORTS ) {
            bool  portExists = false;

            BOOST_FOREACH (DM::Port * p, this->VIBeModule->getInPorts()){
                std::string portname1 = p->getLinkedDataName();
                std::string portname2 = gp->getPortName().toStdString();
                if (portname1.compare(portname2) == 0) {
                    portExists = true;
                }
            }
            if (!portExists) {
                ExistingInPorts.removeAt(ExistingInPorts.indexOf(gp->getPortName()));
                this->ports.remove(i);
                delete gp;
            }
        }
        if (gp->getPortType()  < DM::OUTPORTS ) {
            bool  portExists = false;

            BOOST_FOREACH (DM::Port * p, this->VIBeModule->getOutPorts()){
                if (p->getLinkedDataName().compare(gp->getPortName().toStdString()) == 0) {
                    portExists = true;
                }
            }
            if (!portExists) {
                ExistingOutPorts.removeAt(ExistingOutPorts.indexOf(gp->getPortName()));
                this->ports.remove(i);
                delete gp;
            }
        }

    }





}
void ModelNode::resetModel() {
    DM::Module *oldmodule = this->VIBeModule;
    this->VIBeModule = this->simulation->resetModule(this->VIBeModule->getUuid());

    if(this->VIBeModule==oldmodule)
    {
        DM::Logger(DM::Error) << "The code-base has changed for module \"" << this->VIBeModule->getName() << "\". There is an error in the new code-base";
    }

    foreach(GUIPort * p, this->ports) {
        DM::Port * po = 0;
        if ((this->VIBeModule->getInPort( p->getPortName().toStdString()) == 0)) {
            po = this->VIBeModule->getOutPort( p->getPortName().toStdString());
        } else {
            po = this->VIBeModule->getInPort( p->getPortName().toStdString());
        }
        p->updatePort( po );
    }

}

void ModelNode::addPort(DM::Port * p) {
    if (p->getPortType() < DM::OUTPORTS ) {
        foreach (QString pname, ExistingInPorts) {
            if (pname.compare(QString::fromStdString(p->getLinkedDataName())) == 0) {
                return;
            }            
        }
        ExistingInPorts << QString::fromStdString(p->getLinkedDataName());
    } else {
        foreach (QString pname, ExistingOutPorts) {
            if (pname.compare(QString::fromStdString(p->getLinkedDataName())) == 0) {
                return;
            }
        }
        ExistingOutPorts << QString::fromStdString(p->getLinkedDataName());
    }

    GUIPort * gui_p = new  GUIPort(this, p);
    ports.append(gui_p);
    if  (p->getPortType() < DM::OUTPORTS) {
        gui_p->setPos(this->boundingRect().width(),70+gui_p->boundingRect().height()*this->outputCounter++);
        if (outputCounter > inputCounter)
            this->h = h+20;
    }else {
        gui_p->setPos(0,70+gui_p->boundingRect().height()*this->inputCounter++);
        if (outputCounter < inputCounter)
            this->h = h+20;

    }
}


/*void ModelNode::removePort(int Type, QString s) {
    int counter = 0;
    int itemID;

    this->update();
}*/


//ModelNode

ModelNode::ModelNode( DM::Module *VIBeModule,GUISimulation * simulation)
{
    this->guiPortObserver.setModelNode(this);
    this->guiResultObserver.setResultWidget(this->ResultWidget);
    //VIBeModule->addResultObserver(& this->guiResultObserver);
    this->minimized = false;
    this->visible = true;
    this->setParentItem(0);
    this->outputCounter = 0;
    this->inputCounter = 0;
    this->parentGroup = 0;
    this->VIBeModule = VIBeModule;
    this->id = VIBeModule->getID();
    this->simulation = simulation;
    this->nodes = 0;

    this->setFlag(QGraphicsItem::ItemIsSelectable, true);
    this->setFlag(QGraphicsItem::ItemIsMovable, true);
    this->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    this->simpleTextItem = new QGraphicsSimpleTextItem ("Module: " + QString::fromStdString(VIBeModule->getClassName()));
    double w = this->simpleTextItem->boundingRect().width()+40;

    double w1 = w;
    QGraphicsSimpleTextItem tn ("Name: " + QString::fromStdString(this->VIBeModule->getName()));
    w = w < tn.boundingRect().width() ? tn.boundingRect().width() : w;


    w = w < 140 ? 140 : w;
    l = w+4;
    h =  80;
    VIBeModule->addPortObserver( & this->guiPortObserver);
    this->updatePorts();

    Color = COLOR_MODULE;
}

ModelNode::ModelNode(QGraphicsItem * parent, QGraphicsScene * scene) :QGraphicsItem(parent, scene) {

}


void ModelNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    if(this->isSelected() == true) {
        Color = COLOR_MODULESELECTED;

    } else {
        Color = COLOR_MODULE;
    }

    if(this->visible){
        QPen pen(Qt::black, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);


        QLinearGradient linearGrad(QPointF(0, h), QPointF(0, 0));
        QColor c1 = Color;
        QColor c2 = Color;
        linearGrad.setColorAt(0, c1);
        linearGrad.setColorAt(.4, c2);
        QBrush brush(linearGrad);

        painter->setBrush(Qt::white);
        painter->setPen(pen);
        QPainterPath path;
        path.addRect(0, 0,l,h);
        painter->fillPath(path, brush);
        painter->strokePath(path, pen);

        if (!this->VIBeModule->getName().empty())
            painter->drawText(QPoint(22,35), "Name: " + QString::fromStdString(this->VIBeModule->getName()));

        painter->drawText(QPoint(22,15), "Module: " + QString::fromStdString(this->VIBeModule->getClassName()));
        /*if (this->parentGroup)
            painter->drawText(QPoint(22,55), "Group: "+ QString::fromStdString(this->parentGroup->getVIBeModel()->getName()));*/


    }
}


QRectF ModelNode::boundingRect() const {
    QRect r (0, 0,
             l, h);
    return r;

}

QVariant ModelNode::itemChange(GraphicsItemChange change, const QVariant &value) {
    return QGraphicsItem::itemChange(change, value);
}

ModelNode::~ModelNode() {
    //int index = this->nodes->indexOf(this);
    //this->nodes->remove(index);
    foreach (GUIPort * p, ports) {
        delete p;
        p = 0;
    }
    if (this->VIBeModule != 0)
        this->simulation->removeModule(this->VIBeModule->getUuid());
    this->VIBeModule = 0;

    if (this->parentGroup != 0) {
        this->parentGroup->removeModelNode(this);
    }


    ports.clear();
    delete this->simpleTextItem;

}



void ModelNode::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )  {

    if (this->parentGroup != 0) {
        this->parentGroup->recalculateLandH();
        this->parentGroup->update();
    }

    QGraphicsItem::mouseMoveEvent(event);



}

void ModelNode::mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * event ) {

    if(this->visible){
        this->simulation->updateSimulation();
        if (this->VIBeModule->createInputDialog() == false )
        {

            QWidget * gui  = new GUIModelNode(this->getVIBeModel(), this);
            gui->show();
        }
    }
}
void ModelNode::mousePressEvent ( QGraphicsSceneMouseEvent * event ) {
    /*if(this->minimized){
        QGraphicsItem::mousePressEvent(event );
    }*/
}


GUIPort * ModelNode::getGUIPort(DM::Port * p) {

    foreach (GUIPort * gui_p, this->ports){
        if (gui_p->getVIBePort() == p)
            return gui_p;
    }


    return 0;
}

void ModelNode::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
    QMenu menu;
    QAction  * a_edit = menu.addAction("edit");
    QAction * a_rename = menu.addAction("rename");
    QAction * a_delete = menu.addAction("delete");
    QAction * a_showData = menu.addAction("showData");
    QMenu * GroupMenu =     menu.addMenu("Groups");

    GroupMenu->setTitle("Group");
    QVector<QAction *> actions;
    std::vector<DM::Group*> gs = this->simulation->getGroups();
    BOOST_FOREACH (DM::Group * g, gs) {
        if (this->VIBeModule->getUuid().compare(g->getUuid())) {
            QAction *a = GroupMenu->addAction(QString::fromStdString(g->getName()));
            a->setObjectName(QString::fromStdString(g->getUuid()));
            actions.push_back(a);
        }

    }
    if (this->VIBeModule->getGroup()->getUuid().compare(this->simulation->getRootGroup()->getUuid()) != 0) {
        QAction *a = GroupMenu->addAction("none");
        a->setObjectName(QString::fromStdString(this->simulation->getRootGroup()->getUuid()));
        actions.push_back(a);
    }

    foreach (QAction *a,  actions) {
        connect( a, SIGNAL( activated() ), this, SLOT( addGroup() ), Qt::DirectConnection );
    }

    connect( a_delete, SIGNAL( activated() ), this, SLOT( deleteModelNode() ), Qt::DirectConnection );
    connect( a_edit, SIGNAL( activated() ), this, SLOT( editModelNode() ), Qt::DirectConnection );
    connect( a_rename, SIGNAL(activated() ), this, SLOT( renameModelNode() ), Qt::DirectConnection);
    connect( a_showData, SIGNAL(activated() ), this, SLOT( printData() ), Qt::DirectConnection);
    menu.exec(event->screenPos());

}
void ModelNode::editModelNode() {
    GUIModelNode * gui  = new GUIModelNode(this->getVIBeModel() ,this);
    gui->show();
}

void ModelNode::renameModelNode() {
    QString text =QInputDialog::getText(0, "Name", tr("User name:"), QLineEdit::Normal);
    if (!text.isEmpty())
        this->getVIBeModel()->setName(text.toStdString());

    if (this->isGroup())
    {
        this->simulation->changeGroupName((GroupNode*) this);
    }
}

void ModelNode::deleteModelNode() {

    delete this;
}
void ModelNode::removeGroup() {
    QString name = QObject::sender()->objectName();
    this->update(this->boundingRect());
}

void ModelNode::setMinimized(bool b) {
    if (!this->isGroup() ){
        this->minimized = b;

        bool visible = true;
        if  (b) {
            visible = false;

        }
        this->setVisible(visible);
    }
}


void ModelNode::addGroup() {
    //Find GroupNode
    QString name = QObject::sender()->objectName();
    GroupNode * gn;
    DM::Group * g;
    if (name.compare(QString::fromStdString(this->simulation->getRootGroup()->getUuid())) == 0) {
        this->VIBeModule->setGroup((DM::Group * ) this->simulation->getRootGroup());

        return;
    }

    foreach (DM::Module * m, this->simulation->getModules()) {
        if (m->getUuid().compare(name.toStdString()) == 0) {
            g = (DM::Group *) m;
        }
    }
    if (g == 0)
        return;

    if (this->getVIBeModel()->getGroup()->getUuid().compare(g->getUuid()) == 0) {
        return;
    }



    this->VIBeModule->setGroup(g);

    this->getSimulation()->GUIaddModule(this->VIBeModule, this->pos());
    this->VIBeModule = 0;
    delete this;
}

void ModelNode::printData() {
    DM::Logger(DM::Debug) << this->getVIBeModel()->getName();

    foreach (DM::Port * p, this->getVIBeModel()->getOutPorts())
    {
        std::string dataname = p->getLinkedDataName();
        DM::Logger(DM::Debug) << dataname;
        DM::System * sys = this->getVIBeModel()->getData(dataname);
        if (sys == 0) {
            continue;
        }
        foreach (std::string name, sys->getNamesOfViews()) {
            DM::Logger(DM::Debug) << name;
            DM::View view = sys->getViewDefinition(name);
            DM::Component * c = sys->getComponent(view.getIdOfDummyComponent());
            if (c == 0) {
                continue;
            }
            std::map<std::string,DM::Attribute*> attributes = c->getAllAttributes();
            for (std::map<std::string,DM::Attribute*>::const_iterator it  = attributes.begin(); it != attributes.end(); ++it) {
                DM::Logger(DM::Debug) << it->first;
            }
        }


    }

}
