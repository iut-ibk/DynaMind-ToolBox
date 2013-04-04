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
#include <dmmoduleregistry.h>
#include <modelnode.h>
#include <groupnode.h>
#include <rootgroupnode.h>
#include <dmlogger.h>

#ifndef PYTHON_EMBEDDING_DISABLED
#include <dmpythonenv.h>
#endif

GUISimulation::GUISimulation() : Simulation()
{

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

void GUISimulation::GUIaddModule( DM::Module * m, QPointF pos)
{
	/*
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
	*/
}

void GUISimulation::registerRootNode() {
    //this->GUIaddModule(this->getRootGroup(), QPointF(0,0));
}

void GUISimulation::GUIaddModule(QString name, QPointF pos, DM::Module *group)
{
    //Create Module in DynaMind
	/*
    DM::Module * m = this->addModule(name.toStdString());
    if (!m) {
        DM::Logger(DM::Error) << "Couldn't Create Module " << name.toStdString();
        return;
    }
    m->setGroup((DM::Group*)group);
    this->GUIaddModule(m, pos);
	*/
}



void GUISimulation::updateSimulation()
{    
    //this->startSimulation(true);
}

void GUISimulation::reset()
{
	/*
    this->resetSimulation();
    this->reloadModules();
    this->updateSimulation();
	*/
}

void GUISimulation::clearSimulation() {
	/*
    RootGroupNode * rg = this->getGroupNode((DM::Group*)this->getRootGroup())->getRootGroupNode();

    foreach (ModelNode * m, rg->getChildNodes()) {
        rg->removeModelNode(m);
        m->deleteModelNode();
    }

    this->setSimulationStatus(DM::SIM_OK);
	*/
}

void GUISimulation::showHelp(string classname, string uuid) {
    //emit showHelpForModule(classname, uuid);
}

void GUISimulation::loadModulesFromDefaultLocation()
{
    QVector<QDir> cpv;
    cpv.push_back(QDir(QDir::currentPath() + "/Modules"));
    cpv.push_back(QDir(QDir::currentPath() + "/bin/Modules"));
#if defined DEBUG || _DEBUG
    cpv.push_back(QDir(QDir::currentPath() + "/../Modules/Debug"));
#else
    cpv.push_back(QDir(QDir::currentPath() + "/../Modules/Release"));
    cpv.push_back(QDir(QDir::currentPath() + "/../Modules/RelWithDebInfo"));
    cpv.push_back(QDir(QDir::currentPath() + "/../../../output/Modules/Release"));
    cpv.push_back(QDir(QDir::currentPath() + "/../../../output/Modules/RelWithDebInfo"));
#endif
    
    foreach (QDir cp, cpv)  
	{
        QStringList modulesToLoad = cp.entryList();
        std::cout <<  "loading modules from " << cp.absolutePath().toStdString() << std::endl;
        foreach (QString module, modulesToLoad) 
		{
			if (module == ".." || module == "." || !module.endsWith(".dll"))
                continue;
            //DM::Logger(DM::Debug) << module.toStdString();
            //std::cout <<  module.toStdString() << std::endl;
            QString ml = cp.absolutePath() +"/" + module;
			if (this->getModuleRegistry()->addNativePlugin(ml.toStdString()))
                ;//loadedModuleFiles.push_back(ml.toStdString());
        }
    }

#ifndef PYTHON_EMBEDDING_DISABLED
    QDir cp;
    cp = QDir(QDir::currentPath() + "/bin/PythonModules/scripts");
    loadPythonModulesFromDirectory(cp.absolutePath().toStdString());

    cp = QDir(QDir::currentPath() + "/PythonModules/scripts");
    loadPythonModulesFromDirectory(cp.absolutePath().toStdString());
#endif
}

void GUISimulation::loadPythonModulesFromDirectory(std::string path) {
#ifndef PYTHON_EMBEDDING_DISABLED
    QDir pythonDir = QDir(QString::fromStdString(path));
    QStringList filters;
    filters << "*.py";
    QStringList files = pythonDir.entryList(filters);
    DM::PythonEnv::getInstance()->addPythonPath((path));
    foreach(QString file, files) 
	{
        try
		{
			std::string n = DM::PythonEnv::getInstance()->registerNodes(this->getModuleRegistry(), file.remove(".py").toStdString());
            //loadedModuleFiles.push_back(file.toStdString());
        } 
		catch(...) 
		{
            DM::Logger(DM::Warning)  << "Can't load Module " << file.toStdString();

        }
    }
#endif
}

bool GUISimulation::loadModulesFromSettings() {
    QSettings settings;
    QString text;
    QStringList list;

#ifndef PYTHON_EMBEDDING_DISABLED
    //Init Python
    QStringList pythonhome = settings.value("pythonhome",QStringList()).toString().replace("\\","/").split(",");
    for (int index = 0; index < pythonhome.size(); index++)
        DM::PythonEnv::getInstance()->addPythonPath(pythonhome.at(index).toStdString());

    QDir pythonDir;
    text = settings.value("pythonModules").toString();
    list = text.replace("\\","/").split(",");
    foreach (QString s, list){
        loadPythonModulesFromDirectory(s.toStdString());
    }
#endif

    // Native Modules
    text = settings.value("nativeModules").toString();
    list = text.replace("\\","/").split(",");
    foreach (QString s, list) {
        if (s.isEmpty())
            continue;
        DM::Logger(DM::Standard) << "Loading Native Modules " <<s.toStdString();
        if (getModuleRegistry()->addNativePlugin(s.toStdString())) {
            //loadedModuleFiles.push_back(s.toStdString());
        }
    }

    return true;
}

ModelNode* GUISimulation::guiAddModule(QString moduleName)
{
	DM::Module* m = addModule(moduleName.toStdString());
	ModelNode* node = new ModelNode(m);
	//moduleGuiMap[node] = m;
	return node;
}

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