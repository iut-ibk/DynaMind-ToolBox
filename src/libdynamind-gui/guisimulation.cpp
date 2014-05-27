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
#include <portnode.h>
#include <groupnode.h>
#include <simulationtab.h>
#include <linknode.h>
#include <dmsimulationreader.h>
#include <dmsimulationwriter.h>
#include <simulationio.h>
#include <dmgroup.h>
#include <qtablewidget.h>
#include <dmlogger.h>
#include <QGraphicsView>

#ifndef PYTHON_EMBEDDING_DISABLED
#include <dmpythonenv.h>
#endif

GUISimulation::GUISimulation(QWidget * parent, QTabWidget* tabWidget) : Simulation()
{
	this->parent = parent;
	this->tabWidget = tabWidget;
	currentDocument = "";

	addTab(NULL);
}

void GUISimulation::clearSimulation() 
{
	Simulation::reset();
	clear();
}

DM::Module* GUISimulation::addModule(std::string moduleName, DM::Module* parent, bool callInit)
{
	DM::Module* m = Simulation::addModule(moduleName, parent, callInit);
	if(!m)
		return NULL;
	ModelNode* node = new ModelNode(m, this);

	if(!parent)
		selectTab(0);
	else
	{
		for(int i=1;i<tabs.size();i++)
			if(tabs.at(i)->getParentGroup() == parent)
				selectTab(i);	
	}

	getSelectedTab()->addItem(node);
	modelNodes[m] = node;

	// group stuff
	if(m->isGroup())
	{
		DM::Group* g = dynamic_cast<DM::Group*>(m);
		DM::Logger(DM::Debug) << "added group '" << moduleName << "'";
		SimulationTab* groupTab = addTab(g);
		GroupNode* gnode = new GroupNode(m, this, groupTab, node);
		node->setChild(gnode);
		groupTab->addItem(gnode);
	}
	return m;
}

void GUISimulation::removeModule(DM::Module* m)
{
	// get childs
	std::vector<DM::Module*> toDelete;
	mforeach(ModelNode* child, modelNodes)
		if(child->getModule()->getOwner() == m)
			toDelete.push_back(child->getModule());

	// delete graphical representations
	ModelNode* node = NULL;//modelNodes[m];
	if(map_contains(&modelNodes, m, node))
	{
		// delete entry
		modelNodes.erase(m);
		if(node->getChild())
			delete node->getChild();	// this may be a group node

		m->removeAllObservers();
		delete node;

		// remove from simulation itself
		Simulation::removeModule(m);
	}

	// delete childs
	foreach(DM::Module* child, toDelete)
		removeModule(child);
}

SimulationTab* GUISimulation::addTab(DM::Group* parentGroup)
{
	SimulationTab* tab = new SimulationTab(parent, this, parentGroup);
	tabs.append(tab);
	tabWidget->addTab((QWidget*)tab->getQGViewer(), parentGroup?parentGroup->getClassName():"ROOT");
	return tab;
}

void GUISimulation::closeTab(SimulationTab* tab)
{
	int index = tabs.indexOf(tab);
	closeTab(index);
}
void GUISimulation::closeTab(int index)
{
	if(tabs.size() > index)
	{
		//delete tabs[index]; will cause a crash
		tabWidget->removeTab(index);
		tabs.removeAt(index);
	}
}

SimulationTab* GUISimulation::getTab(int index)
{
	if(tabs.size() > index)
		return tabs[index];
	return NULL;
}
QList<SimulationTab*> GUISimulation::getTabs()
{
	return tabs;
}
SimulationTab* GUISimulation::getSelectedTab()
{
	return getTab(tabWidget->currentIndex());
}

void GUISimulation::selectTab(int index)
{
	tabWidget->setCurrentIndex(index);
}

bool GUISimulation::addLink(DM::Module* source, std::string outPort, 
							DM::Module* dest, std::string inPort, bool checkStream)
{
	if(!Simulation::addLink(source, outPort, dest, inPort, checkStream))
		return false;

	PortNode* inPortNode = getPortNode(dest, inPort, DM::INPORT, dest == source->getOwner());
	PortNode* outPortNode = getPortNode(source, outPort, DM::OUTPORT, source == dest->getOwner());

	if( !inPortNode || !outPortNode )
		return false;

	LinkNode* gl = new LinkNode();

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

PortNode* GUISimulation::getPortNode(DM::Module* m, std::string portName, 
									 DM::PortType type, bool fromInnerGroup)
{
	ModelNode* mn;
	if(map_contains(&modelNodes, m, mn))
	{
		if(!fromInnerGroup)
			return mn->getPort(portName, type);
		//if(PortNode* pn = mn->getPort(portName, type))
		//	return pn;
		else if(mn->getChild())
			return mn->getChild()->getPort(portName, type==DM::INPORT?DM::OUTPORT:DM::INPORT);
	}
	return NULL;
}

bool GUISimulation::removeLink(PortNode* out, PortNode* in)
{
	return Simulation::removeLink(out->getModule(), out->getPortName().toStdString(),
		in->getModule(), in->getPortName().toStdString());
}

bool GUISimulation::loadSimulation(std::string filePath) 
{
	QString qFilePath = QString::fromStdString(filePath);

	std::map<std::string, DM::Module*> modMap;
	QFile file(qFilePath);
	GuiSimulationReader simio(&file);
	std::map<QString, ModuleExEntry> moduleExInfo = simio.getEntries();
	bool result = Simulation::loadSimulation(&file, qFilePath, modMap);

	for(std::map<QString, ModuleExEntry>::iterator it = moduleExInfo.begin();
		it != moduleExInfo.end(); ++it)
	{
		DM::Module* m;
		if(map_contains(&modMap, it->first.toStdString(), m) && m)
			modelNodes[m]->setPos(QPointF(it->second.posX, it->second.posY));
	}

	this->selectTab(0);
	foreach(SimulationTab* tab, tabs)
	{
		QRectF r = tab->itemsBoundingRect();
		tab->getQGViewer()->fitInView(r, Qt::KeepAspectRatio);
		tab->getQGViewer()->centerOn(r.center());
	}

	return result;
}


void GUISimulation::writeSimulation(std::string fileName) 
{
	QFile file(QString::fromStdString(fileName));
	Simulation::writeSimulation(&file, QString::fromStdString(fileName));
	appendGuiInformation(&file, getModules());
}

void GUISimulation::appendGuiInformation(QIODevice* dest, std::list<DM::Module*> modules)
{
	//Write GUI Informations
	DM::Logger(DM::Debug) << "adding GUI information";

	dest->open(QIODevice::Append);
	QTextStream out(dest);
	out << "<DynaMindGUI>\n";
	out << "\t"<<"<GUI_Nodes>\n";

	//Find upper left corner;
	float minx;
	float miny;
	bool first = true;

	foreach(DM::Module* mod, modules)
	{
		ModelNode* m = this->getModelNode(mod);
		if(first)
		{
			minx = m->pos().x();
			miny = m->pos().y();
			first = false;
		}
		else
		{
			minx = min(minx, (float)m->pos().x());
			miny = min(miny, (float)m->pos().y());
		}
	}

	foreach(DM::Module* mod, modules)
	{
		ModelNode* m = this->getModelNode(mod);

		out << "\t\t<GUI_Node>\n";
		out << "\t\t\t<GUI_UUID value=\"" << ADDRESS_TO_INT(mod) << "\"/>\n";
		out << "\t\t\t<GUI_PosX value=\"" << m->scenePos().x() - minx << "\"/>\n";
		out << "\t\t\t<GUI_PosY value=\"" << m->scenePos().y() - miny << "\"/>\n";
		out << "\t\t\t<GUI_Minimized value=\"" << false/*m->isMinimized()*/ << "\"/>\n";
		out << "\t\t</GUI_Node>\n";
	}
	out << "\t</GUI_Nodes>\n";
	out << "</DynaMindGUI>\n";
	out << "</DynaMind>\n";

	dest->close();
	DM::Logger(DM::Debug) << "finished GUI information";
}