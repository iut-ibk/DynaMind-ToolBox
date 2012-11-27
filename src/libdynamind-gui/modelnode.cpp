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
#include <sstream>

#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QInputDialog>

#include "ColorPalette.h"
#include <guimodelnode.h>
#include <guiviewdataformodules.h>
#include <guiport.h>
#include <rootgroupnode.h>
#include <guisimulation.h>
#include <dmsystem.h>
#include <dmviewerwindow.h>

std::string ModelNode::getParameterAsString(std::string name) {
    std::ostringstream val;
    int id = this->getDMModel()->getParameterList()[name];
    if (id == DM::DOUBLE || id == DM::LONG || id == DM::STRING) {
        if (id == DM::DOUBLE)
            val << this->getDMModel()->getParameter<double>(name);
        if (id == DM::LONG)
            val << this->getDMModel()->getParameter<long>(name);
        if (id == DM::STRING)
            val << this->getDMModel()->getParameter<std::string>(name);
        return val.str();
    }
    DM::Logger(DM::Warning) << "Parameter " << name << "not found";
    return "";
}

std::string ModelNode::getGroupUUID() {
    return this->getDMModel()->getGroup()->getUuid();
}

void ModelNode::updatePorts () {

    //Add Ports
    //If Port exists is checked by addPort
    foreach (DM::Port * p, this->getDMModel()->getInPorts()){
        this->addPort(p);
    }
    foreach (DM::Port * p, this->getDMModel()->getOutPorts()) {
        this->addPort(p);
    }

    for (int i = this->ports.size()-1; i > -1; i--) {
        GUIPort * gp = this->ports[i];
        if (!gp->getVIBePort()) {
            if (gp->getPortType() > DM::OUTPORTS) {
                ExistingInPorts.removeAt(ExistingInPorts.indexOf(gp->getPortName()));
                this->inputCounter--;
            } else {
                ExistingOutPorts.removeAt(ExistingOutPorts.indexOf(gp->getPortName()));
                this->outputCounter--;
            }

            this->ports.remove(i);
            delete gp;
            continue;
        }
        if (gp->getVIBePort()->isPortTuple())
            continue;
        if (gp->getPortType()  > DM::OUTPORTS ) {
            bool  portExists = false;

            foreach (DM::Port * p, this->getDMModel()->getInPorts()){
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

            foreach (DM::Port * p, this->getDMModel()->getOutPorts()){
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
    this->update();
}
void ModelNode::resetModel() {
    DM::Module *oldmodule = this->getDMModel();
    this->VIBeModuleUUID = this->simulation->resetModule(this->getDMModel()->getUuid())->getUuid();

    if(this->getDMModel()==oldmodule)
    {
        DM::Logger(DM::Error) << "The code-base has changed for module \"" << this->getDMModel()->getName() << "\". There is an error in the new code-base";
    }

    foreach(GUIPort * p, this->ports) {
        DM::Port * po = 0;
        if ((this->getDMModel()->getInPort( p->getPortName().toStdString()) == 0)) {
            po = this->getDMModel()->getOutPort( p->getPortName().toStdString());
        } else {
            po = this->getDMModel()->getInPort( p->getPortName().toStdString());
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
        gui_p->setPos(this->boundingRect().width(),30+gui_p->boundingRect().height()*this->outputCounter++);
        if (outputCounter > inputCounter)
            this->h = h+20;
    }else {
        gui_p->setPos(0,30+gui_p->boundingRect().height()*this->inputCounter++);
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

DM::Module * ModelNode::getDMModel() {
    return this->simulation->getModuleWithUUID(this->VIBeModuleUUID);
}

ModelNode::ModelNode(DM::Module *VIBeModule, GUISimulation * simulation)
{
    this->guiPortObserver.setModelNode(this);
    this->minimized = false;
    this->visible = true;
    this->setParentItem(0);
    this->outputCounter = 0;
    this->inputCounter = 0;
    this->parentGroup = 0;
    this->VIBeModuleUUID = VIBeModule->getUuid();
    this->id = VIBeModule->getID();
    this->simulation = simulation;
    this->nodes = 0;

    this->setFlag(QGraphicsItem::ItemIsSelectable, true);
    this->setFlag(QGraphicsItem::ItemIsMovable, true);
    this->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    this->simpleTextItem = new QGraphicsSimpleTextItem ("Module: " + QString::fromStdString(VIBeModule->getClassName()));
    double w = this->simpleTextItem->boundingRect().width()+40;

    QGraphicsSimpleTextItem tn ("Name: " + QString::fromStdString(VIBeModule->getName()));
    w = w < tn.boundingRect().width() ? tn.boundingRect().width() : w;


    w = w < 140 ? 140 : w;
    l = w+4;
    h =  35;
    VIBeModule->addPortObserver( & this->guiPortObserver);
    this->updatePorts();

    Color = COLOR_MODULE;
}

ModelNode::ModelNode(QGraphicsItem * parent, QGraphicsScene * scene) :QGraphicsItem(parent, scene) {

}


void ModelNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    if(this->isSelected() == true) {
        Color = COLOR_MODULESELECTED;
    }
    else if (this->getDMModel()->isExecuted()) {
        Color = COLOR_EXECUTED;
    }
    else if (this->getDMModel()->isDebugMode()){
        Color = COLOR_DEBUG;
    }
    else {
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

        if (!this->getDMModel()->getName().empty())
            painter->drawText(QPoint(22,35), "Name: " + QString::fromStdString(this->getDMModel()->getName()));

        painter->drawText(QPoint(22,15), "Module: " + QString::fromStdString(this->getDMModel()->getClassName()));


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

    DM::Module * m = this->getDMModel();


    if (m!=0) {
        foreach (GUIPort * p, ports) {
            delete p;
            p = 0;
        }
    }
    if (this->getDMModel()!= 0)
        this->simulation->removeModule(this->VIBeModuleUUID);

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
        if (this->getDMModel()->createInputDialog() == false )
        {

            QWidget * gui  = new GUIModelNode(this->getDMModel(), this);
            gui->show();
        }
    }
}
void ModelNode::mousePressEvent ( QGraphicsSceneMouseEvent * event ) {
    QGraphicsItem::mousePressEvent(event );
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
    QAction * a_reset = menu.addAction("reset");
    QAction * a_delete = menu.addAction("delete");
    QAction * a_showData = menu.addAction("stream");
    QAction * a_viewData = menu.addAction("viewer");

    QAction * a_showHelp = menu.addAction("help");
    QAction * a_module_debug = 0;
    QAction * a_module_release = 0;
    
    if (this->getDMModel()->isDebugMode())
        a_module_release = menu.addAction("ReleaseMode");
    else
        a_module_debug = menu.addAction("DebugMode");    
    QMenu * GroupMenu =     menu.addMenu("Groups");

    a_viewData->setEnabled(this->getDMModel()->getOutPorts().size() > 0);
    
    GroupMenu->setTitle("Group");
    QVector<QAction *> actions;
    std::vector<DM::Group*> gs = this->simulation->getGroups();
    foreach (DM::Group * g, gs) {
        if (this->getDMModel()->getUuid().compare(g->getUuid())) {
            QAction *a = GroupMenu->addAction(QString::fromStdString(g->getName()));
            a->setObjectName(QString::fromStdString(g->getUuid()));
            actions.push_back(a);
        }

    }
    DM::Module * rgroup = this->simulation->getRootGroup();
    DM::Module * group = this->getDMModel()->getGroup();
    if (group != 0 && rgroup != 0) {
        if (this->getDMModel()->getGroup()->getUuid().compare(this->simulation->getRootGroup()->getUuid()) != 0) {
            QAction *a = GroupMenu->addAction("RootGroup");
            a->setObjectName(QString::fromStdString(this->simulation->getRootGroup()->getUuid()));
            actions.push_back(a);
        }
    }

    foreach (QAction *a,  actions) {
        connect( a, SIGNAL( activated() ), this, SLOT( addGroup() ), Qt::DirectConnection );
    }

    connect( a_delete, SIGNAL( activated() ), this, SLOT( deleteModelNode() ), Qt::DirectConnection );
    connect( a_edit, SIGNAL( activated() ), this, SLOT( editModelNode() ), Qt::DirectConnection );
    connect( a_rename, SIGNAL(activated() ), this, SLOT( renameModelNode() ), Qt::DirectConnection);
    connect( a_showData, SIGNAL(activated() ), this, SLOT( printData() ), Qt::DirectConnection);
    connect( a_viewData, SIGNAL(activated() ), this, SLOT( viewData() ), Qt::DirectConnection);    
    connect( a_module_debug, SIGNAL(activated() ), this, SLOT( setDebug() ), Qt::DirectConnection);
    connect( a_module_release, SIGNAL(activated() ), this, SLOT( setRelease() ), Qt::DirectConnection);
    connect( a_showHelp, SIGNAL(activated() ), this, SLOT( showHelp() ), Qt::DirectConnection);
    connect( a_reset, SIGNAL(activated() ), this, SLOT( setResetModule() ), Qt::DirectConnection);
    menu.exec(event->screenPos());

}
void ModelNode::editModelNode() {
    GUIModelNode * gui  = new GUIModelNode(this->getDMModel() ,this);
    gui->show();
}

void ModelNode::renameModelNode() {
    QString text =QInputDialog::getText(0, "Name", tr("Input:"), QLineEdit::Normal);
    if (!text.isEmpty())
        this->getDMModel()->setName(text.toStdString());

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
    DM::Group * g;
    if (name.compare(QString::fromStdString(this->simulation->getRootGroup()->getUuid())) == 0) {
        this->getDMModel()->setGroup((DM::Group * ) this->simulation->getRootGroup());
        this->getSimulation()->GUIaddModule(this->getDMModel(), this->pos());
        this->VIBeModuleUUID = "";
        delete this;
        return;
    }

    foreach (DM::Module * m, this->simulation->getModules()) {
        if (m->getUuid().compare(name.toStdString()) == 0) {
            g = (DM::Group *) m;
        }
    }
    if (g == 0)
        return;

    if (this->getDMModel()->getGroup()->getUuid().compare(g->getUuid()) == 0) {
        return;
    }

    this->getDMModel()->setGroup(g);
    this->getSimulation()->GUIaddModule(this->getDMModel(), this->pos());
    //Set VIBeModule empty otherwise destructor deletes DynaMind module
    this->VIBeModuleUUID = "";
    delete this;




}

void ModelNode::printData() {

    GUIViewDataForModules * gv = new GUIViewDataForModules(this->getDMModel());
    gv->show();

    
}

void ModelNode::viewData() {
    //TODO hook(er) me up
    DM::Port *p = this->getDMModel()->getOutPorts()[0];
    DM::System *system = this->getDMModel()->getData(p->getLinkedDataName());
    
    DM::ViewerWindow *viewer_window = new DM::ViewerWindow(system);
    viewer_window->show();
}

void ModelNode::showHelp() {
    emit showHelp(this->getDMModel()->getClassName(), this->getDMModel()->getUuid());
    //this->simulation->showHelp();
}

void ModelNode::setDebug() {
    this->getDMModel()->setDebugMode(true);
}

void ModelNode::setRelease() {
    this->getDMModel()->setDebugMode(false);
}

void ModelNode::setResetModule()
{
    this->getDMModel()->setExecuted(false);
    this->simulation->reloadModules();
    this->simulation->updateSimulation();
}
