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
#include <dmmodule.h>
#include <guiport.h>
#include <rootgroupnode.h>
#include <guisimulation.h>
#include <dmsystem.h>
#include <dmviewerwindow.h>
#include <groupnode.h>

void GUIModelObserver::notifyAddPort(const std::string &name, const DM::PortType type)
{
	node->addPort(name, type);
}
void GUIModelObserver::notifyRemovePort(const std::string &name, const DM::PortType type)
{
	node->removePort(name, type);
}

/*
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
}*/
/*
std::string ModelNode::getGroupUUID() {
    return this->getDMModel()->getGroup()->getUuid();
}*/
/*
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
        PortNode * gp = this->ports[i];
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
	
}*/
/*
void ModelNode::resetModel() {
	
    DM::Module *oldmodule = this->getDMModel();
    this->VIBeModuleUUID = this->simulation->resetModule(this->getDMModel()->getUuid())->getUuid();

    if(this->getDMModel()==oldmodule)
    {
        DM::Logger(DM::Error) << "The code-base has changed for module \"" << this->getDMModel()->getName() << "\". There is an error in the new code-base";
    }

    foreach(PortNode * p, this->ports) {
        DM::Port * po = 0;
        if ((this->getDMModel()->getInPort( p->getPortName().toStdString()) == 0)) {
            po = this->getDMModel()->getOutPort( p->getPortName().toStdString());
        } else {
            po = this->getDMModel()->getInPort( p->getPortName().toStdString());
        }
        p->updatePort( po );
    }
	
}*/
/*
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

    PortNode * gui_p = new  PortNode(this, p);
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
}*/


/*void ModelNode::removePort(int Type, QString s) {
    int counter = 0;
    int itemID;

    this->update();
}*/


//ModelNode
/*
DM::Module * ModelNode::getDMModel() {
    //return this->simulation->getModuleWithUUID(this->VIBeModuleUUID);
	return 0;
}*/

ModelNode::ModelNode(DM::Module* m, GUISimulation* sim)
{
	child = NULL;
	/*
    //this->guiPortObserver.setModelNode(this);
    this->minimized = false;
    this->visible = true;
    this->setParentItem(0);
    this->outputCounter = 0;
    this->inputCounter = 0;
    this->parentGroup = 0;
    //this->VIBeModuleUUID = VIBeModule->getUuid();
    //this->id = VIBeModule->getID();
    this->simulation = simulation;
    this->nodes = 0;
	*/
	//this->moduleName = moduleName;
	module = m;
	simulation = sim;
    this->setFlag(QGraphicsItem::ItemIsSelectable, true);
    this->setFlag(QGraphicsItem::ItemIsMovable, true);
    this->setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

	//this->simpleTextItem = new QGraphicsSimpleTextItem ("Module: " + QString::fromStdString(module->getName()));
   // int w = this->simpleTextItem->boundingRect().width()+40;

   /* QGraphicsSimpleTextItem tn ("Name: " + QString::fromStdString(module->getName()));
    w = w < tn.boundingRect().width() ? tn.boundingRect().width() : w;


    w = w < 140 ? 140 : w;
    width = w + 4;*/

	resize();

    //VIBeModule->addPortObserver( & this->guiPortObserver);
    //this->updatePorts();
	/*
	foreach(std::string portName, m->getOutPortNames())
		this->ports.append(new PortNode(QString::fromStdString(portName), this, OUTPORT));
	foreach(std::string portName, m->getInPortNames())
		this->ports.append(new PortNode(QString::fromStdString(portName), this, INPORT));
	*/
    //Color = COLOR_MODULE;

	// be shure to add observer last, as module dimensions are not set before
	new GUIModelObserver(this, m);
}

void ModelNode::resize()
{
	// module name has to fit in node
	QString text = "Name: " + QString::fromStdString(module->getName());
	QRectF textSize = QGraphicsSimpleTextItem(text).boundingRect();
	width = max(150, (int)textSize.width() + 50);
	
	// make groups a bit bigger
	if(!module->isGroup())
		height =  45;
	else
		height =  65;

	// ports names have to fit
	int maxPortSize = 0;
	foreach(PortNode* p, ports)
		maxPortSize = max(maxPortSize, (int)QGraphicsSimpleTextItem(p->getPortName()).boundingRect().width());

	maxPortSize += 20;
	width = max(width, 2*maxPortSize);
	
	// port number has to fit
	int maxPortCount = max(module->getInPortNames().size(), module->getOutPortNames().size());
	height = max(height, 20 + 15 * maxPortCount);

	// update port pos
	foreach(PortNode* p, ports)
		p->updatePos();
}
void ModelNode::setPos(const QPointF &pos)
{
	QGraphicsItem::setPos(pos);
	
	if(DM::Module* m = module->getOwner())
	{
		if(GroupNode* gn = (GroupNode*)getSimulation()->getModelNode(m)->getChild())
		{
			gn->resize();
		}
	}
}

PortNode* ModelNode::getPort(std::string portName, const DM::PortType type)
{
	foreach(PortNode* p, ports)
		if(p->getPortName().toStdString() == portName && p->getType() == type)
			return p;

	return NULL;
}

QVector<PortNode*> ModelNode::getPorts(DM::PortType type)
{
	QVector<PortNode*> ps;
	foreach(PortNode* p, ports)
		if(p->getType() == type)
			ps.push_back(p);

	return ps;
}


void ModelNode::addPort(const std::string &name, const DM::PortType type)
{
	this->ports.append(new PortNode(QString::fromStdString(name), module, type, this, this->simulation));
	resize();
}
void ModelNode::removePort(const std::string &name, const DM::PortType type)
{
	PortNode* p = getPort(name, type);
	if(p)
		ports.erase(find(ports.begin(), ports.end(), p));
	delete p;
	resize();
}

/*
void ModelNode::updatePorts()
{
	// add new ports
	foreach(std::string portName, module->getOutPortNames())
		if(!getPort(portName, DM::OUTPORT))
			this->ports.append(new PortNode(QString::fromStdString(portName), this, DM::OUTPORT));
	foreach(std::string portName, module->getInPortNames())
		if(!getPort(portName, DM::INPORT))
			this->ports.append(new PortNode(QString::fromStdString(portName), this, DM::INPORT));

	std::vector<PortNode*> toRemove;
	foreach(PortNode* n, ports)
	{
		std::string portName = n->getPortName().toStdString();
		DM::PortType type = n->getType();
		if( (!module->hasInPort(portName) && type == DM::INPORT) ||
			(!module->hasOutPort(portName) && type == DM::OUTPORT))
			toRemove.push_back(n);
	}
	foreach(PortNode* n, toRemove)
	{
		ports.erase(find(ports.begin(), ports.end(), n));
		delete n;
	}
	
	// remove deprecated ports
	QVector<int> deprecatedPorts;
	int cnt = 0;

	foreach(PortNode* n, ports)
	{
		if( !module->hasInPort(n->getPortName().toStdString()) || 
			!module->hasOutPort(n->getPortName().toStdString()))
				deprecatedPorts.push_back(cnt);
		cnt++;
	}
	for(int i=0;i<deprecatedPorts.size();i++)
		ports.remove(deprecatedPorts[deprecatedPorts.size()-i-1]);
}*/

/*
ModelNode::ModelNode(QGraphicsItem * parent, QGraphicsScene * scene) :QGraphicsItem(parent, scene) {

}*/


void ModelNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) 
{
	float lineWidth = 2.0f;
	QColor fillcolor;    
	
	switch(module->getStatus())
	{
	//case DM::MOD_OK:				fillcolor = COLOR_MODULE_FINISHED;		break;
	//case DM::MOD_EXECUTIONERROR:	fillcolor = COLOR_MODULE_EXE_ERROR;		break;
	//case DM::MOD_CHECKERROR:		fillcolor = COLOR_MODULE_CHECK_ERROR;	break;

	case DM::MOD_UNTOUCHED:			fillcolor = QColor(200,200,200);	break;

	case DM::MOD_EXECUTING:			fillcolor = QColor(255,255,0);		break;
	case DM::MOD_EXECUTION_OK:		fillcolor = QColor(0,255,0);		break;
	case DM::MOD_EXECUTION_ERROR:	fillcolor = QColor(255,0,0);		break;

	case DM::MOD_CHECK_OK:			fillcolor = QColor(150,255,150);	break;
	case DM::MOD_CHECK_ERROR:		fillcolor = QColor(255,100,100);	break;
	}

    if(this->visible)
	{
		// the constructor for QColor is neccessary
        QPen rectPen(QColor(COLOR_MODULEBORDER), lineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        /*QLinearGradient linearGrad(QPointF(0, h), QPointF(0, 0));
        QColor c1 = COLOR_MODULE;
		QColor c2 = Qt::white;
        linearGrad.setColorAt(0, c1);
        linearGrad.setColorAt(1, c2);*/
        QBrush brush(fillcolor);

        QPainterPath rectPath;
		rectPath.addRect(boundingRect());
        painter->fillPath(rectPath, brush);
        painter->strokePath(rectPath, rectPen);
		
		if(module->isSuccessorMode())
		{
			QPainterPath rectGlowPath;
			QRectF r = boundingRect();
			rectGlowPath.addRect(r.x() + lineWidth,		r.y() + lineWidth, 
								r.width() - 2*lineWidth,	r.height() - 2*lineWidth );
			painter->strokePath(rectGlowPath, 
								QPen(QColor(150,150,255), lineWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
		}
		
		QString text =  "Module: " + QString::fromStdString(module->getName());
		QRectF textSize = QGraphicsSimpleTextItem(text).boundingRect();
		painter->drawText(	(boundingRect().width() - textSize.width())/2,
							textSize.height() + 0,
							text);
    }
	this->update();
}


QRectF ModelNode::boundingRect() const {
    QRect r (0, 0,
             width, height);
    return r;

}

QVariant ModelNode::itemChange(GraphicsItemChange change, const QVariant &value) {
    return QGraphicsItem::itemChange(change, value);
}

ModelNode::~ModelNode() 
{
}


void ModelNode::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )  {
	/*
    if (this->parentGroup != 0) {
        this->parentGroup->recalculateLandH();
        this->parentGroup->update();
    }
	*/
	if(DM::Module* m = module->getOwner())
	{
		if(GroupNode* gn = (GroupNode*)getSimulation()->getModelNode(m)->getChild())
		{
			gn->resize();
			this->scene()->update();
		}
	}
    QGraphicsItem::mouseMoveEvent(event);
}
/*
void ModelNode::showDialog()
{
    if(this->visible)
	{
		QWidget *w = module->getGUI();
        if(w)
			w->show();
		else
			editModelNode();
	}
}*/

void ModelNode::mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * event ) 
{
    //showDialog();
	editModelNode();
}
void ModelNode::mousePressEvent ( QGraphicsSceneMouseEvent * event ) {
    QGraphicsItem::mousePressEvent(event );
}

/*
PortNode * ModelNode::getGUIPort(DM::Port * p) {

    foreach (PortNode * gui_p, this->ports){
        if (gui_p->getVIBePort() == p)
            return gui_p;
    }


    return 0;
}*/

void ModelNode::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
	
    QMenu menu;
	QSignalMapper* signalMapper = new QSignalMapper(this);
	int i=0;
	foreach(string s, module->getOutPortNames())
	{
		QAction* action = menu.addAction(QString::fromStdString("view data at port '"+s+"'"));
		connect( action, SIGNAL(triggered() ), signalMapper, SLOT( map() ));

		signalMapper->setMapping(action, i++);
	}
	connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(viewData(int)));
	
    QAction  * a_edit = menu.addAction("edit");
    QAction * a_delete = menu.addAction("delete");
    QAction * a_showHelp = menu.addAction("show help");
    QAction * a_showData = menu.addAction("show data stream");
    QAction * a_successorMode = menu.addAction("change successor mode");

    connect( a_edit, SIGNAL( triggered() ), this, SLOT( editModelNode() ), Qt::DirectConnection );
    connect( a_delete, SIGNAL( triggered() ), this, SLOT( deleteModelNode() ), Qt::DirectConnection );
    connect( a_showHelp, SIGNAL(triggered() ), this, SLOT( showHelp() ), Qt::DirectConnection);
    connect( a_showData, SIGNAL(triggered() ), this, SLOT( printData() ), Qt::DirectConnection);

	connect( a_successorMode, SIGNAL(triggered() ), this, SLOT( changeSuccessorMode() ), Qt::DirectConnection);

	/*if(module->getGUI())
	{
		QAction * a_showDialog = menu.addAction("module GUI");
		connect( a_showDialog, SIGNAL(triggered() ), this, SLOT( showDialog() ), Qt::DirectConnection);
	}*/
	
    menu.exec(event->screenPos());

	/*
    QAction * a_rename = menu.addAction("rename");
    QAction * a_reset = menu.addAction("reset");
    QAction * a_viewData = menu.addAction("viewer");
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
        connect( a, SIGNAL( triggered() ), this, SLOT( addGroup() ), Qt::DirectConnection );
    }

    connect( a_rename, SIGNAL(triggered() ), this, SLOT( renameModelNode() ), Qt::DirectConnection);
    connect( a_viewData, SIGNAL(triggered() ), this, SLOT( viewData() ), Qt::DirectConnection);
    connect( a_module_debug, SIGNAL(triggered() ), this, SLOT( setDebug() ), Qt::DirectConnection);
    connect( a_module_release, SIGNAL(triggered() ), this, SLOT( setRelease() ), Qt::DirectConnection);
    connect( a_reset, SIGNAL(triggered() ), this, SLOT( setResetModule() ), Qt::DirectConnection);
    menu.exec(event->screenPos());
	*/
}

void ModelNode::editModelNode() 
{
	if(this->visible)
	{
		if(!module->createInputDialog())
			(new GUIModelNode(module ,this))->show();
	}
}
/*
void ModelNode::renameModelNode() {
    QString text =QInputDialog::getText(0, "Name", tr("Input:"), QLineEdit::Normal);
    if (!text.isEmpty())
        this->getDMModel()->setName(text.toStdString());
	
    if (this->isGroup())
    {
        this->simulation->changeGroupName((GroupNode*) this);
    }
}*/

void ModelNode::deleteModelNode() 
{
	getSimulation()->removeModule(getModule());
}
/*
void ModelNode::removeGroup() {
    QString name = QObject::sender()->objectName();
    this->update(this->boundingRect());
}*/
/*
void ModelNode::setMinimized(bool b) {
    if (!this->isGroup() ){
        this->minimized = b;

        bool visible = true;
        if  (b) {
            visible = false;

        }
        this->setVisible(visible);
    }
}*/

/*
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
*/

void ModelNode::printData() 
{
    GUIViewDataForModules * gv = new GUIViewDataForModules(module);
    gv->show();
}


void ModelNode::changeSuccessorMode()
{
	module->setSuccessorMode(!module->isSuccessorMode());
}

void ModelNode::viewData(int portIndex) 
{
    //TODO hook(er) me up
    //DM::Port *p = this->getDMModel()->getOutPorts()[0];
    //DM::System *system = this->getDMModel()->getData(p->getLinkedDataName());

	DM::System *system = module->getOutPortData(module->getOutPortNames()[portIndex]);
    DM::ViewerWindow *viewer_window = new DM::ViewerWindow(system);
    viewer_window->show();
}

#include <guihelpviewer.h>

void ModelNode::showHelp() 
{
    //emit showHelp(module->getClassName());
    //this->simulation->showHelp();

	GUIHelpViewer* ghv = new GUIHelpViewer(simulation);
	ghv->showHelpForModule(module);
    ghv->show();
}
/*
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
*/