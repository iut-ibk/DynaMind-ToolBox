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
#include <QString>
#include <QFileInfo>

#include <boost/foreach.hpp>
#include <iostream>
#include <map>
#include <sstream>
#include "datamanagement.h"
#include "simulation.h"
#include "simulationreader.h"
#include "module.h"
#include "rootmodule.h"
#include "moduleregistry.h"
#include "moduleparameterreader.h"
#include "vector"
#include <boost/python.hpp>
#include "moduleregistry.h"
#include <QThread>
#include "simulationobserver.h"
#include <module.h>
#include <modulelink.h>
#include <port.h>
#include <idatabase.h>
#include <moduleregistry.h>
#include <QSettings>
#include <QDir>
#include <group.h>
#include <simulaitonwriter.h>
#include <simulationreader.h>
#include <omp.h>
#include <vibe_logger.h>
#include <porttuple.h>
#include <pythonenv.h>

namespace vibens {
struct SimulationPrivate {
    //ModuleContainer * root;
    ModuleRegistry registry;

    long simulationCounter;
    std::map<long,Module * > ModuleMap;
    std::string workingDirectory;
    std::vector<std::string> pythonModules;
    std::string filename;
    DataObserver * observer;
    SimulationObserver * simObserver;
    int counter;
};




Module * Simulation::addModule(std::string ModuleName) {

    Module *module = this->moduleRegistry->createModule(ModuleName);
    Logger(Debug) << "Add Module" << ModuleName << " " << module->getUuid();
    module->setSimulation(this);
    if (module->getGroup() == 0)
        module->setGroup(this->rootGroup);
    this->Modules[module->getUuid()] = module;
    module->setSimulation(this);
    return module;


}

IDataBase * Simulation::getDataBase() {
    return this->database;
}


Simulation::~Simulation() {
    Logger(Debug)  << "Remove Modules";
    delete this->rootGroup;

    delete data;
}

Simulation::Simulation(std::string fileName, std::vector<std::string> pythonModules) {
    this->setTerminationEnabled(true);
    data = new SimulationPrivate();
    data->simObserver = 0;
    data->workingDirectory = QFileInfo(QString::fromStdString(fileName)).absolutePath().toStdString();
    data->pythonModules = pythonModules;
    data->filename = fileName;


}
void Simulation::reloadModules() {
    //Init Python
    QSettings settings("IUT", "VIBe2");
    Logger(Standard) << "Reload Modules";

    QStringList pythonhome = settings.value("pythonhome",QStringList()).toString().replace("\\","/").split(",");
    for (int index = 0; index < pythonhome.size(); index++)
        vibens::PythonEnv::getInstance()->addPythonPath(pythonhome.at(index).toStdString());



    QDir pythonDir;
    QString text = settings.value("pythonModules").toString();
    QStringList list = text.replace("\\","/").split(",");
    //settings.endGroup();
    foreach (QString s, list){
        vibens::PythonEnv::getInstance()->addPythonPath(s.toStdString());
        pythonDir = QDir(s);
        QStringList filters;
        filters << "*.py";
        QStringList files = pythonDir.entryList(filters);
        foreach(QString file, files) {
            try{
                std::string n = vibens::PythonEnv::getInstance()->registerNodes(moduleRegistry, file.remove(".py").toStdString());
                Logger(Debug) << n;

            } catch(...) {
                Logger(Warning)  << "Can't load Module " << file.toStdString();
                std::cout << file.toStdString() << std::endl;
            }
        }

    }
}

Simulation::Simulation() {
    this->setTerminationEnabled(true);
    data = new SimulationPrivate();
    data->simObserver = 0;
    data->observer = 0;
    data->counter = 0;
    this->rootGroup = new RootGroup();
    this->rootGroup->setSimulation(this);
    //this->Modules[rootGroup.getUuid()] = &this->rootGroup;
    this->moduleRegistry = new ModuleRegistry();
    //vibens::PythonEnv::getInstance()->getInstance()->addOverWriteStdCout();
    QSettings settings("IUT", "VIBe2");

    //Init Python
    QStringList pythonhome = settings.value("pythonhome",QStringList()).toString().replace("\\","/").split(",");
    //for (int index = 0; index < pythonhome.size(); index++)
        //vibens::PythonEnv::getInstance()->addPythonPath(pythonhome.at(index).toStdString());


}

void Simulation::registerNativeModules(string Filename) {
    Logger(Standard) << "Loading Native Modules " << Filename ;
    moduleRegistry->addNativePlugin(Filename);
}

void Simulation::registerPythonModules(std::string path) {
    vibens::PythonEnv::getInstance()->addPythonPath(path);
    QDir pythonDir = QDir(QString::fromStdString(path));
    QStringList filters;
    filters << "*.py";
    QStringList files = pythonDir.entryList(filters);
    foreach(QString file, files) {
        //try{
            Logger(Debug) << "Loading Python module: " << file.remove(".py").toStdString();
            std::string n = vibens::PythonEnv::getInstance()->registerNodes(moduleRegistry, file.remove(".py").toStdString());

        //} catch(...) {
        //    Logger(Warning)  << "Can't load Module: " << file.toStdString();
        //    std::cout << file.toStdString() << std::endl;
        //}
    }
}

ModuleRegistry * Simulation::getModuleRegistry() {
    return this->moduleRegistry;
}

void Simulation::addSimulationObserver(SimulationObserver * simulationObserver) {
    data->simObserver = simulationObserver;
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

    BOOST_FOREACH(Module * m, this->getModules()) {
        if (m->isGroup())
            groups.push_back((Group * ) m);
    }

    return groups;
}

void Simulation::resetModules() {
    this->database->resetDataBase();
    foreach (Module * m, this->getModules()) {
        this->resetModule(m->getUuid());
    }
}

void Simulation::run(bool check) {
    this->registerDataBase(DataManagement::getInstance().getDataBase());
    Logger(Standard) << "Run Simulation";
    bool isConnected = true;
    if (check)
        isConnected = this->checkConnections();
    if (isConnected) {
        Logger(Debug) << "Reset Steps";
        this->rootGroup->resetSteps();
        Logger(Debug) << "Start Simulations";
        this->rootGroup->run();
    }
    Logger(Standard) << "End Simulation";
}
void Simulation::registerDataBase(IDataBase * database) {
    this->database = database;
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
    BOOST_FOREACH (Module * m, this->getModules()) {
        if (name.compare(m->getName()) == 0)
            return m;
    }
}

Module * Simulation::getModuleWithUUID(std::string UUID) {
    BOOST_FOREACH (Module * m, this->getModules()){
        if (UUID.compare(m->getUuid()) == 0)
            return m;
    }
    return 0;
}
std::vector<Module * > Simulation::getModulesFromType(std::string name) {
    std::vector<Module * > ress;
    foreach (Module * m, this->getModules()) {
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

    foreach (LinkEntry le, simreader.getLinks()) {
        std::string outPortUUID = UUIDTranslator[le.OutPort.UUID.toStdString()];
        std::string inPortUUID = UUIDTranslator[le.InPort.UUID.toStdString()];
        vibens::Port * p_out = 0;
        vibens::Port * p_in = 0;
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
bool Simulation::checkConnections() const {
    Logger(Debug) << "Check Connections ";
    foreach(Module * m, this->getModules()) {
        std::vector<Port *> ports   = m->getInPorts();
        foreach(Port * p, ports) {
            if (p->getLinks().size() < 1) {
                Logger(Error) << m->getUuid() << " " << m->getName() << "Module Not fully Connected";
                return false;
            }
        }
        //Check Double Ports

        if (m->isGroup()) {
            Group * g = (Group *) m;
            std::vector<PortTuple*>  pts = g->getInPortTuples();

            foreach (PortTuple * pt, pts) {
                Port * p = pt->getInPort();
                if (p->getLinks().size() < 1) {
                    Logger(Error) << m->getUuid() << " " << m->getName() << "Module Not fully Connected";
                    return false;
                }
            }
            pts = g->getOutPortTuples();
            foreach (PortTuple * pt, pts) {
                Port * p = pt->getInPort();
                if (p->getLinks().size() < 1) {
                    Logger(Error) << m->getUuid() << " " << m->getName() << "Module Not fully Connected";
                    return false;
                }
            }
        }


    }
    Logger(Debug) << "Check Connections finished";
    return true;
}

Module * Simulation::resetModule(std::string UUID) {
    Logger(Debug) << "Reset Module " << UUID;
    Module * m = this->getModuleWithUUID(UUID);
    Module * new_m = this->addModule(m->getClassName());
    //Modules.erase(std::find(Modules.begin(), Modules.end(),new_m));
    new_m->copyParameterFromOtherModule(m);



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


    //std::replace(Modules.begin(), Modules.end(), m, new_m);
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
std::vector<Module*> Simulation::getModules() const{
    std::vector<Module*> ms;

    for (std::map<std::string, Module*>::const_iterator it = this->Modules.begin(); it != this->Modules.end(); ++it)
        ms.push_back(it->second);
    return ms;
}
}


