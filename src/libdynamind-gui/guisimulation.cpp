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

#include "guisimulation.h"
//#include <dmmoduleregistry.h>
#include <modelnode.h>
#include <guiport.h>
//#include <groupnode.h>
#include <rootgroupnode.h>
//#include <dmlogger.h>

#include <guilink.h>
#include <dmsimulationreader.h>
#include <simulationio.h>

#include <dmgroup.h>
#include <qtablewidget.h>

#ifndef PYTHON_EMBEDDING_DISABLED
#include <dmpythonenv.h>
#endif

GUISimulation::GUISimulation(QWidget * parent, QTabWidget* tabWidget) : Simulation()
{
	this->parent = parent;
	this->tabWidget = tabWidget;

	selectedTab = addTab("ROOT");
}

/*
void GUISimulation::changeGroupName(GroupNode * g) {
    emit GroupNameChanged(g);
}

GroupNode * GUISimulation::getGroupNode(DM::Group * g) {

    foreach (GroupNode * gn, this->groupNodes) {
        if (gn->getDMModel() == g)
            return gn;
    }

    return 0;
}*/
/*
void GUISimulation::GUIaddModule( DM::Module * m, QPointF pos)
{
	
    //Create Visual Representation of the Module
    if (!m->isGroup()) {
        ModelNode * node = new ModelNode(m, this);
        this->modelNodes.append(node);
        node->setPos(pos);
        connect(node, SIGNAL(showHelp(std::string, std::string)), this, SLOT(showHelp(std::string, std::string)));
        emit addedModule(node);
    }
    if (m->isGroup()) {
        GroupNode * node = new GroupNode(m, this);
        this->groupNodes.append(node);
        node->setPos(pos);
        connect(node, SIGNAL(showHelp(std::string, std::string)), this, SLOT(showHelp(std::string, std::string)));
        emit addedGroup(node);
    }

    this->updateSimulation();
	
}*/
/*
void GUISimulation::registerRootNode() {
    this->GUIaddModule(this->getRootGroup(), QPointF(0,0));
}
*/
/*
void GUISimulation::GUIaddModule(QString name, QPointF pos, DM::Module *group)
{
    //Create Module in DynaMind
	
    DM::Module * m = this->addModule(name.toStdString());
    if (!m) {
        DM::Logger(DM::Error) << "Couldn't Create Module " << name.toStdString();
        return;
    }
    m->setGroup((DM::Group*)group);
    this->GUIaddModule(m, pos);
	
}
*/

/*
void GUISimulation::updateSimulation()
{    
    //this->startSimulation(true);
}

void GUISimulation::resetSimulation()
{
	this->resetSimulation();
    this->reloadModules();
    this->updateSimulation();
}*/

void GUISimulation::clearSimulation() 
{
	Simulation::reset();
	mforeach(ModelNode* m, modelNodes)
		m->deleteModelNode();
	modelNodes.clear();

	for(int i = tabs.size()-1; i>=0;i--)
		closeTab(i);

	clear();

	/*
    RootGroupNode * rg = this->getGroupNode((DM::Group*)this->getRootGroup())->getRootGroupNode();

    foreach (ModelNode * m, rg->getChildNodes()) {
        rg->removeModelNode(m);
        m->deleteModelNode();
    }

    this->setSimulationStatus(DM::SIM_OK);
	*/
}

void GUISimulation::showHelp(string classname) 
{
    //emit showHelpForModule(classname);
}
/*
QVector<QDir> defaultModuleDirectories()
{
	QVector<QDir> cpv;
	cpv.push_back(QDir::currentPath() + "/Modules");
	cpv.push_back(QDir::currentPath() + "/bin/Modules");
#if defined DEBUG || _DEBUG
	cpv.push_back(QDir::currentPath() + "/../Modules/Debug");
#else
	cpv.push_back(QDir::currentPath() + "/../Modules/Release");
	cpv.push_back(QDir::currentPath() + "/../Modules/RelWithDebInfo");
	cpv.push_back(QDir::currentPath() + "/../../../output/Modules/Release");
	cpv.push_back(QDir::currentPath() + "/../../../output/Modules/RelWithDebInfo");
#endif

#ifndef PYTHON_EMBEDDING_DISABLED
	cpv.push_back(QDir::currentPath() + "/bin/PythonModules/scripts");
	cpv.push_back(QDir::currentPath() + "/PythonModules/scripts");
#endif
	return cpv;
}

void GUISimulation::loadModulesFromDefaultLocation()
{
	foreach (QDir cp, defaultModuleDirectories())  
		registerModulesFromDirectory(cp);
}
*/
/*
ModelNode* GUISimulation::guiAddModule(QString moduleName)
{
	return modelNodes[addModule(moduleName.toStdString())];
}*/


DM::Module* GUISimulation::addModule(std::string moduleName, bool callInit)
{
	DM::Module* m = Simulation::addModule(moduleName, callInit);
	if(!m)
		return NULL;
	ModelNode* node = new ModelNode(m, this);
	//lastAddedModuleNode->setPos(-100, -50);
	getSelectedTab()->addItem(node);
	modelNodes[m] = node;

	// group stuff
	DM::Group* g = dynamic_cast<DM::Group*>(m);	// dont forget to check vtable!
	if(g)
	{
		DM::Logger(DM::Debug) << "added group";

	}
	return m;
}

SimulationTab* GUISimulation::addTab(QString name)
{
	SimulationTab* tab = new SimulationTab(parent, this);
	tabs.append(tab);
	tabWidget->addTab((QWidget*)tab->getQGViewer(), name);
	return tab;
}

void GUISimulation::closeTab(int index)
{
	if(tabs.size() > index)
	{
		delete tabs[index];
		tabs.removeAt(index);
		tabWidget->removeTab(index);
	}
}
SimulationTab* GUISimulation::getTab(int index)
{
	if(tabs.size() > index)
		return tabs[index];
	return NULL;
}
SimulationTab* GUISimulation::getSelectedTab()
{
	return selectedTab;
}

void GUISimulation::SelectTab(int index)
{
	selectedTab = getTab(index);
	if(!selectedTab)	// safty net, select root
		selectedTab = getTab(0);
}

/*bool GUISimulation::addLink(PortNode* out, PortNode* in)
{
	return Simulation::addLink(out->getModule(), out->getPortName().toStdString(),
								in->getModule(), in->getPortName().toStdString());
}*/

bool GUISimulation::addLink(DM::Module* source, std::string outPort, 
							DM::Module* dest, std::string inPort, bool checkStream)
{
	if(!Simulation::addLink(source, outPort, dest, inPort, checkStream))
		return false;

	PortNode* inPortNode = getPortNode(dest, inPort, INPORT);
	PortNode* outPortNode = getPortNode(source, outPort, OUTPORT);

	if( !inPortNode || !outPortNode )
		return false;

	GUILink* gl = new GUILink();
	
	if( !gl )
		return false;

	inPortNode->scene()->addItem(gl);
	gl->setInPort(inPortNode);
	gl->setOutPort(outPortNode);
	
	// backlink for modelNode position update
	inPortNode->addLink(gl);
	outPortNode->addLink(gl);

	return true;
}

PortNode* GUISimulation::getPortNode(DM::Module* m, std::string portName, PortType type)
{
	ModelNode* mn;
	if(map_contains(&modelNodes, m, mn))
			return mn->getPort(portName, type);

	return NULL;
}

bool GUISimulation::removeLink(PortNode* out, PortNode* in)
{
	return Simulation::removeLink(out->getModule(), out->getPortName().toStdString(),
								in->getModule(), in->getPortName().toStdString());
}
/*
SimulationTab* GUISimulation::addTab(QWidget *parent)
{
	SimulationTab* tab = new SimulationTab(parent, this);
	tabs.append(tab);
	return tab;
}
void GUISimulation::closeTab(int i)
{
	if(i >= tabs.size() || i < 0)
		return;

	delete tabs[i];
	tabs.removeAt(i);
}
SimulationTab* GUISimulation::getTab(int i)
{
	if(i >= tabs.size() || i < 0)
		return NULL;

	return tabs[i];
}*/

/*void GUISimulation::guiUpdatePorts(ModelNode* node)
{
	DM::Module* m;
	if(map_contains(&moduleGuiMap, node, m))
	{
		// reset ports
		node->inPorts.clear();
		node->outPorts.clear();
		// get names from DM::Module
		foreach(std::string portName, m->getInPortNames())
			node->inPorts.push_back(QString::fromStdString(portName));

		foreach(std::string portName, m->getOutPortNames())
			node->outPorts.push_back(QString::fromStdString(portName));
	}
}*/



bool GUISimulation::loadSimulation(std::string filename) 
{
	GuiSimulationReader simio(QString::fromStdString(filename));
	std::map<QString, ModuleExEntry> moduleExInfo = simio.getEntries();
	
	std::map<std::string, DM::Module*> modMap;
	bool result = Simulation::loadSimulation(filename, modMap);

	for(std::map<QString, ModuleExEntry>::iterator it = moduleExInfo.begin();
		it != moduleExInfo.end(); ++it)
	{
		DM::Module* m;
		if(map_contains(&modMap, it->first.toStdString(), m))
			modelNodes[m]->setPos(it->second.posX, it->second.posY);
	}
	return result;
}


void GUISimulation::writeSimulation(std::string fileName) 
{
	Simulation::writeSimulation(fileName);

	//Write GUI Informations
	DM::Logger(DM::Debug) << "adding GUI information";

	QFile file(QString::fromStdString(fileName));
	file.open(QIODevice::Append);
	QTextStream out(&file);
	out << "<DynaMindGUI>\n";
	out << "\t"<<"<GUI_Nodes>\n";

	//Find upper left corner;
	float minx;
	float miny;
	bool first = true;
	mforeach(ModelNode* m, this->modelNodes)
	{
		if (first)        
		{
			minx = m->pos().x();
			miny = m->pos().y();
			first = false;
		}
		else
		{
			minx = min(minx, (float)m->pos().x());
			miny = max(miny, (float)m->pos().y());
		}
	}

	mforeach(ModelNode* m, this->modelNodes)
	{
		QString mid = QString::fromAscii((char*)m->getModule(), sizeof(DM::Module*));

		out << "\t\t<GUI_Node>\n";
		out << "\t\t\t<GUI_UUID value=\"" << mid << "\"/>\n";
		out << "\t\t\t<GUI_PosX value=\"" << m->scenePos().x() - minx << "\"/>\n";
		out << "\t\t\t<GUI_PosY value=\"" << m->scenePos().y() - miny << "\"/>\n";
		out << "\t\t\t<GUI_Minimized value=\"" << m->isMinimized() << "\"/>\n";
		out << "\t\t</GUI_Node>\n";
	}
	out << "\t</GUI_Nodes>\n";
	out << "</DynaMindGUI>\n";
	out << "</DynaMind>\n";

	file.close();
}