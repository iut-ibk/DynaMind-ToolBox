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
#include "module.h"
#include <list>
#include <boost/shared_ptr.hpp>
#include <map>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <QString>
#include <QStringList>
#include <typeinfo>
#include <boost/foreach.hpp>
#include <boost/python/wrapper.hpp>
#include <port.h>

#include <QUuid>
#include <sstream>
#include <group.h>
#include <idatabase.h>
#include <modulelink.h>
#include <simulation.h>
#include <datamanagement.h>
#include <portobserver.h>
#include <sstream>
#include <DMcomponent.h>
#include <DMsystem.h>

/**
* @class DM::Module
*
*
* @ingroup DynaMind-Core
*
* @brief Basic module class
* @section Implementation
* @todo write stuff here
* @section Development
* To create a new DynaMind module the new module has to be derived from the module class
*
* @subsection C++
*
*
* Example Code
*
* mymodule.h
* @code
*
* #include "module.h"
*
* using namespace vibens;
* class DM_HELPER_DLL_EXPORT MyModule : public Module
* {
*     DM_DECLARE_NODE(MyModule)
*     public:
*         MyModule();
*         void run();
* };
* @endcode
*
* mymodule.cpp
* @code
*
* #include "mymodule.h"
*
* DM_DECLARE_NODE_NAME(MyModule, MyModuleGroup)
* MyModule::MyModule()
* {
* }
* void MyModule::run();
* {
* }
* @endcode
*
* To add the Module to DynaMind
* mymodules.cpp
* @code
* #include "nodefactory.h"
* #include "moduleregistry.h"
* #include "mymodule.h"
* #include "mymodule2.h"
* extern "C" void DM_HELPER_DLL_EXPORT  registerModules(ModuleRegistry *registry) {
*      registry->addNodeFactory(new NodeFactory<MyModule>());
*      registry->addNodeFactory(new NodeFactory<MyModule2>());
* }
* @endcode
* @author Christian Urich
*
*/
using namespace std;
namespace DM {
struct ModulePrivate {

};

void Module::addPortObserver(PortObserver *portobserver) {
    this->portobserver.push_back(portobserver);
}
void Module::resetParameter() {
    Logger(Debug) << "Reset Parameter";
    this->internalCounter = 0;
}
Module::Module() {

    srand((unsigned)time(0));
    this->uuid = QUuid::createUuid().toString().toStdString();
    this->group = 0;
    init_called = false;
    PythonModule = false;
    internalCounter = 0;
    InPorts = std::vector<Port*>();
    OutPorts = std::vector<Port*>();
    name = "";
    urlToHelpFile = "";
    description = "";
    portobserver = std::vector<PortObserver *>();
    resultobserver = std::vector<ResultObserver * >();

    simulation = 0;


}

Module::~Module() {
    Logger(Debug) << "Remove " << this->getClassName() << " " << this->getUuid();
    if(!this)
        return;
    this->getSimulation()->deregisterModule(this->getUuid());
    this->Destructor();


}
Port * Module::getInPort( std::string name)  {
    for (std::vector<Port*>::const_iterator it = this->InPorts.begin(); it != this->InPorts.end(); ++it) {
        Port * p = *it;
        if (p->getLinkedDataName().compare(name) == 0) {
            return p;
        }

    }
    return 0;
}
Port * Module::getOutPort(std::string name) {
    for (std::vector<Port*>::iterator it = this->OutPorts.begin(); it != this->OutPorts.end(); ++it) {
        Port * p = *it;
        if (p->getLinkedDataName().compare(name) == 0) {
            return p;
        }

    }
    return 0;
}

/**
  *@brief update parameter and data used in the module.
  *
  * Based on the data used in the module the pointer to the data are updated.
  * If the module reads data from, the method tries to get the system from linked module.
  * Therefore the getSystemData method is called. If the module changes the data a new state of the system
  * is created.
  *
  */
void Module::updateParameter() {
    for (boost::unordered_map<std::string,int>::const_iterator it = parameter.begin(); it != parameter.end(); ++it) {
        std::string s = it->first;
        if (it->second != DM::SYSTEM) {
            continue;
        }
        std::vector<DM::View> views= this->views[s];

        //check for reads data. For read access only no new system state is created.
        foreach (DM::View view,  views)  {
            if ( !view.reads() )
                continue;

            //Default links are not fulfilled
            this->data_vals[view.getName()] = 0;
            this->getInPort(view.getName())->setFullyLinked(false);
            if (this->getOutPort(view.getName()) != 0)
                this->getOutPort(view.getName())->setFullyLinked(false);

            DM::System * sys = this->getSystemData(view.getName());

            //Check if all Data for read are really avalible
            if (sys == 0)
                continue;

            //Get View saved in System
            DM::View checkView = sys->getViewDefinition(view.getName());

            //Check Type
            if (checkView.getType() != view.getType())
                continue;

            //Get DummyComponent
            DM::Component * c = sys->getComponent(checkView.getIdOfDummyComponent());
            //Check if attributes are avalible
            if (c == 0) {
                continue;
            }

            //Check if attributes to read are avalible in component
            foreach (std::string a, view.getReadAttributes()) {
                std::map<std::string, DM::Attribute*> existing_attributes = c->getAllAttributes();
                if (existing_attributes.find(a) == existing_attributes.end()) {
                    sys = 0;
                    break;
                }
            }
            if (sys == 0) {
                continue;
            }

            //All Checks successful -> update data
            this->data_vals[view.getName()] = sys;
            this->getInPort(view.getName())->setFullyLinked(true);
            if (this->getOutPort(view.getName()) != 0)
                this->getOutPort(view.getName())->setFullyLinked(true);
        }

        //update data for read and write
        foreach (DM::View view,  views)  {
            if (!view.writes())
                continue;

            //Creats a new Dataset for a complet new dataset
            if (!view.reads()) {
                this->data_vals[view.getName()] = this->getSystem_Write(view);
                this->getOutPort(view.getName())->setFullyLinked(true);
                continue;
            }
            //Create new system state for data that get modified
            if (view.reads()) {
                DM::System * sys_old = this->data_vals[view.getName()];
                if (sys_old != 0) {
                    this->data_vals[view.getName()] = sys_old->createSuccessor();
                    this->data_vals[view.getName()]->addView(view);
                }
            }
        }
    }
}

void Module::setParameterValue(std::string name, std::string v) {
    //Check if parameter exists
    bool exists = false;
    for (boost::unordered_map<std::string, int>::iterator it = parameter.begin(); it != parameter.end(); ++it) {
        std::string paramName = it->first;
        if (paramName.compare(name) == 0)
            exists = true;

    }
    if (!exists) {
        Logger(Error) << "Parameter " << name << "does not exist" ;
        return;

    }

    QString value = QString::fromStdString(v);
    if(parameter[name] == DM::LONG) {
        long * ref = (long * )this->parameter_vals[name];
        if (ref == 0)
            return;
        *ref =  value.toLong();
        return;
    }
    if(parameter[name] == DM::INT) {
        int * ref = (int * )this->parameter_vals[name];
        if (ref == 0)
            return;
        *ref =  value.toInt();
        return;
    }
    if(parameter[name] == DM::BOOL) {
        bool * ref = (bool * )this->parameter_vals[name];
        if (ref == 0)
            return;
        *ref =  (bool) value.toInt();
        return;
    }
    if(parameter[name] == DM::DOUBLE) {
        double * ref = (double * )this->parameter_vals[name];
        if (ref == 0)
            return;
        *ref =  value.toDouble();
        return;
    }
    if(parameter[name] == DM::STRING || parameter[name] == DM::FILENAME) {
        std::string * ref = (std::string * )this->parameter_vals[name];
        if (ref == 0)
            return;
        *ref =  value.toStdString();
        return;
    }
    if(parameter[name] == DM::STRING_LIST) {
        std::vector<std::string> * ref = (std::vector<std::string> * )this->parameter_vals[name];
        if (ref == 0)
            return;
        QStringList list = value.split("*|*");
        foreach(QString s, list) {
            ref->push_back(s.toStdString());
        }

        return;
    }
    if (parameter[name] == DM::STRING_MAP) {
        std::map<std::string, std::string> * ref = (std::map<std::string, std::string> *)this->parameter_vals[name];
        if (ref == 0)
            return;
        QStringList list = value.split("*||*");
        foreach(QString s, list) {
            if (! s.isEmpty()) {
                QStringList list2 = s.split("*|*");
                ref->insert( std::pair<std::string,std::string>(list2[0].toStdString(),list2[1].toStdString()) );
            }
        }
        return;
    }

}


void Module::setParameter() {
    this->internalCounter++;

}

/**
  * @brief Returns the parameter as string value
  *
  * As seperator for STRING_LIST *|* is used and for maps also *||*
  *
  * 1*|*2*|*3*|4*||*
  *
  * 5*|*6*|*7*|*8*||*
  *
*/
std::string Module::getParameterAsString(std::string Name) {
    int ID = this->parameter[Name];
    std::stringstream ss;
    ss.precision(16);
    if (ID == DM::DOUBLE)
        ss << this->getParameter<double>(Name);
    if (ID == DM::INT)
        ss << this->getParameter<int>(Name);
    if (ID == DM::BOOL)
        ss << this->getParameter<bool>(Name);
    if (ID == DM::STRING || ID  == DM::FILENAME)
        ss << this->getParameter<std::string>(Name);
    if (ID == DM::LONG)
        ss << this->getParameter<long>(Name);
    if (ID == DM::STRING_LIST) {
        std::vector<std::string> vec = this->getParameter<std::vector<std::string> >(Name);
        foreach (std::string s, vec) {
            ss << s << "*|*" ;
        }

    }
    if (ID == DM::STRING_MAP) {
        std::map<std::string, std::string> map = this->getParameter<std::map<std::string, std::string> >(Name);
        for (std::map<std::string, std::string>::iterator it = map.begin(); it != map.end(); ++it) {
            ss << it->first << "*|*" << it->second << "*||*" ;
        }

    }

    return ss.str();
}

/**
  * @brief Used to define the data that are used in the module.
  *
  * The data are defined as a vetor of views. For every view in the vector a port according to the AccessType is created.
  * - AccessType Read: Inport
  * - AccessType Write: Outport
  * - AccessType Modify: In and Outport
  *
  * The name of the port is defined by the name of the View. If a port already exists no new port is added.
  *
  **/
void Module::addData(std::string name,  std::vector<DM::View> views) {

    foreach (View v, views) {
        this->data_vals[v.getName()] = 0;
    }

    this->parameterList.push_back(name);
    this->views[name] = views;
    this->parameter[name] = DM::SYSTEM;

    //Add Ports
    foreach (DM::View view,  views)  {
        if (view.reads())
            this->addPort(view.getName(), DM::INSYSTEM);
    }
    foreach (DM::View view,  views)  {
        if (view.writes())
            this->addPort(view.getName(), DM::OUTSYSTEM);
    }
}

/**
  * @brief Returns a pointer to the system where the view is stored
  *
  * The pointer to the system is updated by the updateParameter method. The updateParameter
  * method is always called before the run method.
  */
DM::System* Module::getData(std::string dataname)
{
    return this->data_vals[dataname];
}

std::map<std::string, std::vector<DM::View> > Module::getViews() {
    return this->views;
}

DM::RasterData* Module::getRasterData(string dataname, const  DM::View & v) {

    DM::System * sys = data_vals[dataname];
    DM::View view =  sys->getViewDefinition(v.getName());
    return (RasterData*) sys->getComponent(view.getIdOfDummyComponent());

}

void Module::addParameter(std::string name,int type, void * ref, std::string description) {

    this->parameter[name] = type;
    this->parameter_vals[name] = ref;
    this->parameterList.push_back(name);
    this->parameter_description[name] = description;

}

void Module::Destructor() {
    if (this->group != 0) {
        BOOST_FOREACH(Port * p, this->InPorts) {
            delete p;
        }
        BOOST_FOREACH(Port * p, this->OutPorts) {
            delete p;
        }
        this->InPorts.clear();
        this->OutPorts.clear();

        this->group->removeModule(this);
        this->group = 0;
    }


}

void Module::convertValus(void * value, int Type, QString val) {

    if (Type == DM::INT) {
        int * v = (int*) value;
        *(v) =  val.toInt();
        return;
    }
    if (Type == DM::LONG) {
        long * v = (long*) value;
        *(v) =  val.toLong();
        return;
    }
    if (Type == DM::DOUBLE) {
        double * v = (double*) value;
        *(v) =  val.toDouble();
        return;
    }
    if (Type== DM::STRING || Type== DM::FILENAME) {
        std::string * v = (std::string *) value;
        *(v) =  val.toStdString();
        return;
    }

    return;
}
void Module::removePort(std::string LinkedDataName, int PortType) {
    Logger(Debug) << "RemovePort" << LinkedDataName;

    if (PortType < DM::OUTPORTS) {
        std::vector<Port *>::iterator portToDelete;
        for (std::vector<Port * >::iterator it = this->OutPorts.begin(); it != this->OutPorts.end(); ++it) {
            Port * p = *it;
            if (p->getLinkedDataName().compare(LinkedDataName) == 0)
                portToDelete = it;
        }
        Logger(Debug) << "Delete Port" << LinkedDataName;
        Port * pd = *portToDelete;
        this->OutPorts.erase(portToDelete);
        delete pd;
    } else {
        std::vector<Port * >::iterator portToDelete;
        for (std::vector<Port * >::iterator it = this->InPorts.begin(); it != this->InPorts.end(); ++it) {
            Port * p = *it;
            if (p->getLinkedDataName().compare(LinkedDataName) == 0)
                portToDelete = it;
        }
        Logger(Debug) << "Delete Port" << LinkedDataName;
        Port * pd = *portToDelete;
        this->InPorts.erase(portToDelete);
        delete pd;
    }
    foreach(PortObserver * po, this->portobserver) {
        po->changedPorts();
    }

}
void Module::addPort(std::string LinkedDataName, int PortType) {
    Logger(Debug) << "AddPort" << LinkedDataName;
    Port * p = new Port(this, PortType, LinkedDataName);
    if (PortType < DM::OUTPORTS) {
        this->OutPorts.push_back(p);
    } else {
        this->InPorts.push_back(p);
    }
    foreach(PortObserver * po, this->portobserver) {
        po->changedPorts();
    }

}
std::vector<Port*> Module::getInPorts() {
    return this->InPorts;
}
std::vector<Port*> Module::getOutPorts() {
    return this->OutPorts;
}

void Module::init() {

}

/**
  * @brief Returns a pointer to the system that is linked to the inport of the module.
  * If no system can be found the method returns 0
  *
  * I ports have to be linked to a standard link and in additionally to a back link. If the internal counter of a the module is 0 (the module is)
  * called the first time. The method tries to get the data from the standard link. If the counter > 0 the back link is used.
  *
  */
DM::System*   Module::getSystemData(const std::string &name)  {
    Port * p = this->getInPort(name);
    if (p->getLinks().size() == 0)
        return 0;

    int LinkId = -1;
    int BackId = -1;
    int counter = 0;

    //Identify Positions in the Link Vector
    foreach (ModuleLink * l, p->getLinks()) {
        if (!l->isBackLink()) {
            LinkId = counter;
        } else {
            BackId = counter;
        }
        counter++;
    }
    if (LinkId < 0)
        return 0;
    ModuleLink *l = p->getLinks()[LinkId];
    if (this->internalCounter > 0 && BackId != -1){
        l = p->getLinks()[BackId];
        Logger(Debug) << "BackLink for " << name;
    }
    Logger(Debug) << "BackLink for " << l->getInPort()->getLinkedDataName();


    Module * m = this->simulation->getModuleWithUUID(l->getUuidFromOutPort());
    return m->getSystemState(l->getDataNameFromOutPort());


}

/** @brief Returns the current system state
  *
  * If the succeeding module changes the data a new state of the system has to be created!
  */
DM::System* Module::getSystemState(const std::string &name)
{
    DM::System  * sys = this->data_vals[name];
    if (sys == 0) {
        return 0;
    }
    return sys;

}

/**
  * @brief Creates a new system and adds the corresponding view
  */
DM::System*   Module::getSystem_Write(View view)  {
    DM::System * sys = new DM::System(view.getName());
    sys->addView(view);
    return sys;
}


void Module::sendImageToResultViewer(std::string filename) {
    BOOST_FOREACH(ResultObserver * ro, resultobserver) {
        ro->addResultImage(this->uuid,filename);
    }
}
void Module::sendRasterDataToResultViewer(std::map<std::string , std::string > maps) {
    QVector<RasterData> r;
    for (std::map<std::string, std::string>::iterator it=maps.begin(); it != maps.end(); ++it) {
        std::string uuid_name = it->first;
        r.append(DataManagement::getInstance().getDataBase()->getRasterData(uuid_name.substr(0,uuid_name.find("%")), it->second));
    }

    BOOST_FOREACH(ResultObserver * ro, resultobserver) {

        //ro->addRasterDataToViewer(r);
    }
}



void Module::sendDoubleValueToPlot(double x, double y) {
    BOOST_FOREACH(ResultObserver * ro, resultobserver) {
        ro->addDoubleDataToPlot(this->uuid, x, y);

    }
}


void Module::createDoubleData(std::string name) {
    DataManagement::getInstance().getDataBase()->createDoubleData(this->uuid, name);
}




void Module::setDoubleData(const std::string &name, const double v) {
    simulation->getDataBase()->setDoubleData(uuid, name, v);
}

double Module::getDoubleData(const std::string &name)  {
    Port * p = this->getInPort(name);
    p->getLinks();

    int LinkId = -1;
    int BackId = -1;
    int counter = 0;
    foreach (ModuleLink * l, p->getLinks()) {
        if (!l->isBackLink()) {
            LinkId = counter;
        } else {
            BackId = counter;
        }
        counter++;
    }
    ModuleLink *l = p->getLinks()[LinkId];
    if (this->internalCounter > 0 && BackId != -1){
        l = p->getLinks()[BackId];
    }
    return simulation->getDataBase()->getDoubleData(l->getUuidFromOutPort(), l->getDataNameFromOutPort(), true, l->isBackLinkInChain());
}


void Module::setID(const int id) {
    this->id = id;
}

int Module::getID() const {
    return id;
}


void Module::setGroup(Group *group) {
    if (this->group==0) {
        this->group = group;
    } else {
        this->group->removeModule(this);
        this->group = group;
    }
    this->group->addModule(this);
}
Group * Module::getGroup() {
    return this->group;
}
void Module::setSimulation(Simulation *simulation) {
    this->simulation = simulation;
}

void Module::copyParameterFromOtherModule(Module * m) {
    std::string name_origin = m->getClassName();
    std::string name_this = this->getClassName();
    if (name_origin.compare(name_this) == 0) {
        boost::unordered_map<std::string, int> parameterList = m->getParameterList();

        for ( boost::unordered_map<std::string, int>::iterator it = parameterList.begin(); it != parameterList.end(); ++it) {
            if (it->second < DM::USER_DEFINED_INPUT) {
                this->setParameterValue(it->first, m->getParameterAsString(it->first));
            }
        }

        //Set Group
        this->setGroup(m->getGroup());
        this->name = m->getName();
        foreach(PortObserver * p, m->getPortObserver()) {
            this->portobserver.push_back(p);
        }
        foreach(ResultObserver * p, m->getResultObserver()) {
            this->resultobserver.push_back(p);
        }
    } else {
        Logger(Error) << "Can't Copy Model Parameter from different Type of Module";
    }

}

void Module::printParameterList()  {
    std::vector<std::string> names = this->getParameterListAsVector();
    BOOST_FOREACH (std::string name , names) {
        Logger(Debug) << name << "\t" << this->getParameterAsString(name);
    }

}
}
