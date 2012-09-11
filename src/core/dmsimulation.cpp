/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2011-2012  Christian Urich

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
#include <QString>
#include <QFileInfo>

#include <iostream>
#include <map>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>

#include "dmsimulation.h"
#include "dmsimulationreader.h"
#include "dmmodule.h"
#include "dmmoduleregistry.h"
#include "dmmoduleparameterreader.h"
#include "dmmoduleregistry.h"
#include "dmsimulationobserver.h"
#include <dmmodule.h>
#include <dmmodulelink.h>
#include <dmport.h>
#include <dmmoduleregistry.h>
#include <dmgroup.h>
#include <dmsimulationwriter.h>
#include <dmlogger.h>
#include <dmporttuple.h>
#include <dmpythonenv.h>

#include <QSettings>
#include <QDir>
#include <QThread>

namespace DM {
struct SimulationPrivate {
    ModuleRegistry registry;

    long simulationCounter;
    std::map<long,Module * > ModuleMap;
    std::string workingDirectory;
    std::vector<std::string> pythonModules;
    std::string filename;
    DataObserver * observer;
    std::vector<SimulationObserver *>  simObserver;
    int counter;

    /** Is used to indicate if simulation run was successful. Parameter is set to ture before a Simulation
      * starts. If the parameter is false at the end of the simulation something happend in between
      */
    int simulationStatus;
};




Module * Simulation::addModule(std::string ModuleName) {

    Module *module = this->moduleRegistry->createModule(ModuleName);

    if(!module)
    {
        Logger(Error) << "Not able to add new module" << ModuleName;
        return 0;
    }

    Logger(Debug) << "Add Module" << ModuleName << " " << module->getUuid();
    module->setSimulation(this);
    if (module->getGroup() == 0)
        module->setGroup(this->rootGroup);
    this->Modules[module->getUuid()] = module;
    module->setSimulation(this);
    module->init();
    return module;


}

Simulation::~Simulation() {
    Logger(Debug)  << "Remove Modules";
    delete this->rootGroup;

    delete data;
    delete moduleRegistry;
}

void Simulation::reloadModules() {
    //Init Python
    QSettings settings("IUT", "DYNAMIND");
    Logger(Standard) << "Reload Modules";

    QStringList pythonhome = settings.value("pythonhome",QStringList()).toString().replace("\\","/").split(",");
    for (int index = 0; index < pythonhome.size(); index++)
        DM::PythonEnv::getInstance()->addPythonPath(pythonhome.at(index).toStdString());



    QDir pythonDir;
    QString text = settings.value("pythonModules").toString();
    QStringList list = text.replace("\\","/").split(",");

    foreach (QString s, list){
        DM::PythonEnv::getInstance()->addPythonPath(s.toStdString());
        pythonDir = QDir(s);
        QStringList filters;
        filters << "*.py";
        QStringList files = pythonDir.entryList(filters);
        foreach(QString file, files) {
            try{
                std::string n = DM::PythonEnv::getInstance()->registerNodes(moduleRegistry, file.remove(".py").toStdString());
                Logger(Debug) << n;

            } catch(...) {
                Logger(Warning)  << "Can't load Module " << file.toStdString();
                std::cout << file.toStdString() << std::endl;
            }
        }

    }
}
void Simulation::loadModulesFromDefaultLocation()
{
    QVector<QDir> cpv;
    cpv.push_back(QDir(QDir::currentPath() + "/Modules"));
     cpv.push_back(QDir(QDir::currentPath() + "/bin/Modules"));

    foreach (QDir cp, cpv)  {
        QStringList modulesToLoad = cp.entryList();
        std::cout <<  cp.absolutePath().toStdString() << std::endl;
        foreach (QString module, modulesToLoad) {
            if (module == ".." || module == ".")
                continue;
            DM::Logger(DM::Debug) << module.toStdString();
            std::cout <<  module.toStdString() << std::endl;
            QString ml = cp.absolutePath() +"/" + module;
            if (this->moduleRegistry->addNativePlugin(ml.toStdString())) {
                loadedModuleFiles.push_back(ml.toStdString());
            }
        }
    }
    QDir cp;
    cp = QDir(QDir::currentPath() + "/bin/PythonModules/scripts");
    loadPythonModulesFromDirectory(cp.absolutePath().toStdString());

    cp = QDir(QDir::currentPath() + "/PythonModules/scripts");
    loadPythonModulesFromDirectory(cp.absolutePath().toStdString());
}
void Simulation::loadPythonModulesFromDirectory(std::string path) {
    QDir pythonDir = QDir(QString::fromStdString(path));
    QStringList filters;
    filters << "*.py";
    QStringList files = pythonDir.entryList(filters);
    DM::PythonEnv::getInstance()->addPythonPath((path));
    foreach(QString file, files) {
        try{
            std::string n = DM::PythonEnv::getInstance()->registerNodes(moduleRegistry, file.remove(".py").toStdString());
            loadedModuleFiles.push_back(file.toStdString());
        } catch(...) {
            Logger(Warning)  << "Can't load Module " << file.toStdString();

        }
    }
}

bool Simulation::addModulesFromSettings() {


    QSettings settings("IUT", "DYNAMIND");

    //Init Python
    QStringList pythonhome = settings.value("pythonhome",QStringList()).toString().replace("\\","/").split(",");
    for (int index = 0; index < pythonhome.size(); index++)
        DM::PythonEnv::getInstance()->addPythonPath(pythonhome.at(index).toStdString());

    QDir pythonDir;
    QString text = settings.value("pythonModules").toString();
    QStringList list = text.replace("\\","/").split(",");
    foreach (QString s, list){
        loadPythonModulesFromDirectory(s.toStdString());
    }

    // Native Modules
    text = settings.value("nativeModules").toString();
    list = text.replace("\\","/").split(",");
    foreach (QString s, list) {
        if (s.isEmpty())
            continue;
        Logger(Standard) << "Loading Native Modules " <<s.toStdString();
        if (moduleRegistry->addNativePlugin(s.toStdString())) {
            loadedModuleFiles.push_back(s.toStdString());
        }
    }

    return true;
}

Simulation::Simulation() {
    srand((unsigned)time(NULL));
    this->setTerminationEnabled(true);
    data = new SimulationPrivate();
    //data->simObserver;
    data->observer = 0;
    data->counter = 0;
    this->rootGroup = new DMRootGroup();
    this->rootGroup->setSimulation(this);
    //this->Modules[rootGroup.getUuid()] = &this->rootGroup;
    this->moduleRegistry = new ModuleRegistry();

}

bool Simulation::registerNativeModules(string Filename) {
    Logger(Standard) << "Loading Native Modules " << Filename ;
    return moduleRegistry->addNativePlugin(Filename);
}

void Simulation::registerPythonModules(std::string path) {
    DM::PythonEnv::getInstance()->addPythonPath(path);
    QDir pythonDir = QDir(QString::fromStdString(path));
    QStringList filters;
    filters << "*.py";
    QStringList files = pythonDir.entryList(filters);
    foreach(QString file, files) {
        Logger(Debug) << "Loading Python module: " << file.remove(".py").toStdString();
        std::string n = DM::PythonEnv::getInstance()->registerNodes(moduleRegistry, file.remove(".py").toStdString());

    }
}

ModuleRegistry * Simulation::getModuleRegistry() {
    return this->moduleRegistry;
}

void Simulation::addSimulationObserver(SimulationObserver * simulationObserver) {
    data->simObserver.push_back(simulationObserver);
}

void Simulation::addDataObserver(DataObserver * observer) {
    data->observer = observer;
}
ModuleLink * Simulation::addLink( Port *OutPort, Port *InPort) {
    if (OutPort == 0 || InPort == 0)
        return 0;
    Logger(Debug) << "Add Link" << OutPort->getLinkedDataName() << "-"<< InPort->getLinkedDataName();
    ModuleLink*  ml = new ModuleLink(InPort, OutPort);
    return ml;

}
void Simulation::removeLink(ModuleLink * l) {
    Logger(Debug) << "Remove Link";
    delete l;
}

std::vector<Group*> Simulation::getGroups() {

    std::vector<Group*> groups;

    foreach(Module * m, this->getModules()) {
        if (m->isGroup())
            groups.push_back((Group * ) m);
    }

    return groups;
}

void Simulation::resetModules() {
    std::vector<DM::Module *> mv= this->getModules();
    foreach (Module * m, mv) {
        if (!m->isExecuted())
            this->resetModule(m->getUuid());
    }
}
void Simulation::run() {
    this->resetModules();
    this->startSimulation(false);

}

bool Simulation::startSimulation(bool virtualRun) {

    if (!virtualRun)
        this->startSimulation(true);
    this->data->simulationStatus = SIM_OK;
    this->virtualRun = virtualRun;
    Logger(Standard) << "Run Simulation";
    Logger(Debug) << "Reset Steps";
    this->rootGroup->resetSteps();
    Logger(Debug) << "Start Simulations";
    if (this->rootGroup->getModules().size() > 0) {
        this->rootGroup->run();
    }

    if (!virtualRun) {
        Logger(Standard) << "End Simulation";
        return true;
    }

    //Execute Simulation Observer
    foreach (SimulationObserver * so, data->simObserver) {
        so->VirtualRunDone();
    }
    return true;

}

void Simulation::removeModule(std::string UUid) {
    for(std::map<std::string, Module*>::const_iterator it = Modules.begin(); it != Modules.end(); ++it) {
        if (!it->first.compare(UUid)) {
            delete it->second;
            return;
        }
    }

}
void Simulation::deregisterModule(std::string UUID) {

    if(!this)
        return;

    for(std::map<std::string, Module*>::iterator it = Modules.begin(); it != Modules.end(); ++it) {
        std::string id = it->first;
        if (id.compare(UUID) == 0 ) {
            Modules.erase(it);
            return;
        }

    }

}

void Simulation::writeSimulation(std::string filename) {
    SimulaitonWriter::writeSimulation(filename, this);
}
Module * Simulation::getModuleByName(std::string name) {
    foreach(Module * m, this->getModules()) {
        if (name.compare(m->getName()) == 0)
            return m;
    }
    return 0;
}

Module * Simulation::getModuleWithUUID(std::string UUID) {
    foreach(Module * m, this->getModules()){
        if (UUID.compare(m->getUuid()) == 0)
            return m;
    }

    if (this->getRootGroup()->getUuid().compare(UUID) == 0) {
        return this->getRootGroup();
    }
    return 0;
}
std::vector<Module * > Simulation::getModulesFromType(std::string name) {
    std::vector<Module * > ress;
    foreach(Module * m, this->getModules()) {
        std::string n(m->getClassName());
        if (n.compare(name) == 0)
            ress.push_back(m);
    }

    return ress;
}

std::map<std::string, std::string>  Simulation::loadSimulation(std::string filename) {
    std::map<std::string, std::string> UUIDTranslator;

    SimulationReader simreader(QString::fromStdString(filename));
    foreach (ModuleEntry me, simreader.getModules()) {
        Module * m = this->addModule(me.ClassName.toStdString());
        if (!m) {
            this->setSimulationStatus(SIM_FAILED_LOAD);
            return std::map<std::string, std::string>();
        }

        m->setName(me.Name.toStdString());
        UUIDTranslator[me.UUID.toStdString()] = m->getUuid();
        foreach(QString s, me.ParemterList.keys()) {
            m->setParameterValue(s.toStdString(), me.ParemterList[s].toStdString());
        }
    }
    //Reconstruct Groups;
    foreach (ModuleEntry me, simreader.getModules()) {
        if (me.GroupUUID != simreader.getRootGroupUUID()) {
            Module * m = this->getModuleWithUUID(UUIDTranslator[me.UUID.toStdString()]);
            Group * g = (Group *)this->getModuleWithUUID(UUIDTranslator[me.GroupUUID.toStdString()]);
            m->setGroup(g);
        }
    }

    //Call init Functions of the modules
    foreach (ModuleEntry me, simreader.getModules()) {
        Module * m = this->getModuleWithUUID(UUIDTranslator[me.UUID.toStdString()]);
        m->init();
    }

    foreach (LinkEntry le, simreader.getLinks()) {
        std::string outPortUUID = UUIDTranslator[le.OutPort.UUID.toStdString()];
        std::string inPortUUID = UUIDTranslator[le.InPort.UUID.toStdString()];
        DM::Port * p_out = 0;
        DM::Port * p_in = 0;
        if (!inPortUUID.empty() && !outPortUUID.empty() ) {

            if (le.OutPort.isTuplePort == 1) {
                Group * g = (Group*) this->getModuleWithUUID(outPortUUID);
                bool isWithinThisGroup = false;
                if (g->getUuid().compare(this->getModuleWithUUID(inPortUUID)->getGroup()->getUuid() ) == 0) {
                    isWithinThisGroup = true;
                }
                if (!isWithinThisGroup)     {
                    PortTuple *pt =  g->getOutPortTuple(le.OutPort.PortName.toStdString());
                    if (pt == 0) {
                        Logger(Error) << "OutPorttuple " << le.OutPort.PortName.toStdString() <<"doesn't exist";
                        continue;
                    }
                    p_out = pt->getOutPort();
                } else {
                    PortTuple *pt =  g->getInPortTuple(le.OutPort.PortName.toStdString());
                    if (pt == 0) {
                        Logger(Error) << "OutPorttuple " << le.OutPort.PortName.toStdString() <<"doesn't exist";
                        continue;
                    }
                    p_out = pt->getOutPort();

                }

            }else {

                p_out = this->getModuleWithUUID(outPortUUID)->getOutPort(le.OutPort.PortName.toStdString());
            }
            if (le.InPort.isTuplePort == 1) {
                //Check if Connected Module is within this group
                Group * g = (Group*) this->getModuleWithUUID(inPortUUID);
                bool isWithinThisGroup = false;
                if (g->getUuid().compare(this->getModuleWithUUID(outPortUUID)->getGroup()->getUuid() ) == 0) {
                    isWithinThisGroup = true;
                }
                if (!isWithinThisGroup){
                    PortTuple *pt =  g->getInPortTuple(le.InPort.PortName.toStdString());
                    if (pt == 0) {
                        Logger(Error) << "InPorttuple " << le.InPort.PortName.toStdString() <<"doesn't exist";
                        continue;
                    }
                    p_in = pt->getInPort();
                } else {
                    PortTuple *pt =  g->getOutPortTuple(le.InPort.PortName.toStdString());
                    if (pt == 0) {
                        Logger(Error) << "InPorttuple " << le.InPort.PortName.toStdString() <<"doesn't exist";
                        continue;
                    }
                    p_in = pt->getInPort();
                }
            } else {
                p_in = this->getModuleWithUUID(inPortUUID)->getInPort(le.InPort.PortName.toStdString());
            }
            ModuleLink * l = this->addLink(p_out, p_in);

            if (l != 0)
                l->setBackLink(le.backlink);
        }
    }

    return UUIDTranslator;

}

Module * Simulation::resetModule(std::string UUID) {
    Logger(Debug) << "Reset Module " << UUID;
    Module * m = this->getModuleWithUUID(UUID);
    if(!m)
        return 0;

    Module * new_m =  this->moduleRegistry->createModule(m->getClassName());
    new_m->setSimulation(this);
    if(!new_m)
        return m;


    new_m->copyParameterFromOtherModule(m);

    //Need to call the init function from the module after the parameters are copied to create the ports
    new_m->init();


    foreach(Port * p, m->getInPorts()) {
        foreach(ModuleLink * l, p->getLinks()) {
            std::string name = l->getInPort()->getLinkedDataName();
            l->setInPort(new_m->getInPort(name));
            p->removeLink(l);
        }

    }
    foreach(Port * p, m->getOutPorts()) {
        foreach(ModuleLink * l, p->getLinks()) {
            std::string name = l->getOutPort()->getLinkedDataName();
            l->setOutPort(new_m->getOutPort(name));
            p->removeLink(l);
        }
    }

    if (m->isGroup()) {
        Group * g = (Group *) m;
        Group * new_g= (Group *) new_m;

        foreach (PortTuple *  pt, g->getInPortTuples()) {
            std::string name = pt->getName();
            PortTuple * pt_new = new_g->getInPortTuple(name);
            Port * p = pt->getInPort();
            foreach(ModuleLink * l, p->getLinks()) {
                l->setInPort(pt_new->getInPort());
                p->removeLink(l);
            }
            p = pt->getOutPort();
            foreach(ModuleLink * l, p->getLinks()) {
                l->setOutPort(pt_new->getOutPort());
                p->removeLink(l);
            }
        }

        foreach (PortTuple *  pt, g->getOutPortTuples()) {
            std::string name = pt->getName();
            PortTuple * pt_new = new_g->getOutPortTuple(name);
            Port * p = pt->getInPort();
            foreach(ModuleLink * l, p->getLinks()) {
                l->setInPort(pt_new->getInPort());
                p->removeLink(l);
            }
            p = pt->getOutPort();
            foreach(ModuleLink * l, p->getLinks()) {
                l->setOutPort(pt_new->getOutPort());
                p->removeLink(l);
            }
        }
        foreach (Module * m, g->getModules()) {
            m->setGroup(new_g);
        }
        g->clearModules();


    }

    delete m;
    Modules[new_m->getUuid()] = new_m;
    return new_m;


}
std::vector<ModuleLink*> Simulation::getLinks() {
    std::vector<ModuleLink*> links;
    foreach (Module * m, this->getModules()) {
        foreach(Port * p, m->getInPorts()) {
            foreach(ModuleLink * l, p->getLinks()) {
                links.push_back(l);
            }
        }

        if (m->isGroup()) {
            Group * g = (Group *) m;
            foreach (PortTuple *  pt, g->getInPortTuples()) {
                Port * p = pt->getInPort();
                foreach(ModuleLink * l, p->getLinks()) {
                    links.push_back(l);
                }

            }
            foreach (PortTuple *  pt, g->getOutPortTuples()) {
                Port * p = pt->getInPort();
                foreach(ModuleLink * l, p->getLinks()) {
                    links.push_back(l);
                }
            }
        }
    }

    return links;
}

int Simulation::getSimulationStatus() {
    return this->data->simulationStatus;
}

void Simulation::setSimulationStatus(int Status) {
    this->data->simulationStatus = Status;
}

std::vector<std::string> Simulation::getLoadModuleFiles()
{
    return this->loadedModuleFiles;
}

std::vector<Module*> Simulation::getModules() const{
    std::vector<Module*> ms;

    for (std::map<std::string, Module*>::const_iterator it = this->Modules.begin(); it != this->Modules.end(); ++it)
        ms.push_back(it->second);
    return ms;
}
}


