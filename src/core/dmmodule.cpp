/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @author  Markus Sengthaler <m.sengthaler@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2011  Christian Urich, Markus Sengthaler

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
#include "dmmodule.h"
#include <list>
#include <map>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <QString>
#include <QStringList>
#include <typeinfo>
#include <dmport.h>

#include <QUuid>
#include <sstream>
#include <dmgroup.h>
#include <dmmodulelink.h>
#include <dmsimulation.h>
#include <dmportobserver.h>
#include <sstream>
#include <dmcomponent.h>
#include <dmsystem.h>
#include <dmdatavalidation.h>
#include <dmrasterdata.h>
#include <algorithm>

using namespace std;
namespace DM {
struct ModulePrivate {

};

void Module::addPortObserver(PortObserver *portobserver) {
    this->portobserver.push_back(portobserver);
}
void Module::resetParameter() {
    this->setExecuted(false);
    Logger(Debug) << this->getUuid() <<" Reset Parameter";
    this->internalCounter = 0;
    deep_delete(&ownedSystems_prev);
    deep_delete(&ownedSystems);
}
Module::Module() {    
    this->uuid = QUuid::createUuid().toString().toStdString();
    this->group = 0;
    PythonModule = false;
    internalCounter = 0;
    InPorts = std::vector<Port*>();
    OutPorts = std::vector<Port*>();
    name = "";
    portobserver = std::vector<PortObserver *>();
    resultobserver = std::vector<ResultObserver * >();
    simulation = 0;
    hasBeenExecuted = false;
    debugMode = false;
    _isFullyLinked = false;
}

Module::~Module() {
    resetParameter();

    Logger(Debug) << "Remove " << this->getClassName() << " " << this->getUuid();

    this->getSimulation()->deregisterModule(this->getUuid());
    this->Destructor();
}
Port * Module::getInPort(std::string name) const 
{
    foreach(Port* p, InPorts)
        if(p->getLinkedDataName() == name)
            return p;
    return 0;
}
Port * Module::getOutPort(std::string name) const 
{	
    foreach(Port* p, OutPorts)
        if(p->getLinkedDataName() == name)
            return p;
    return 0;
}
bool Module::checkPreviousModuleUnchanged() {
    Logger(Debug) << this->getUuid() <<" Update Parameter";
    for (std::map<std::string,int>::const_iterator it = parameter.begin(); it != parameter.end(); ++it)
    {
        if (it->second != DM::SYSTEM)
            continue;

        std::string s = it->first;
        std::vector<DM::View> views= this->views[s];
        //Check Reads
        if (!DataValidation::isVectorOfViewRead(views))
            continue;

        const DM::System * sys = this->getConstSystemData(s);
        if (sys == 0 || !sys->getLastModule() || !sys->getLastModule()->isExecuted())
            return false;
    }
    return true;
}

void Module::setInternalCounter(int counter)
{
    this->internalCounter = counter;
}

void Module::updateParameter() {
    //TODO: Clenaing this whole section
    Logger(Debug) << this->getUuid() <<" Update Parameter";
    this->_isFullyLinked = false;

    //If module just has an outport it's always fully linked
    if (this->InPorts.size() == 0)
        this->_isFullyLinked = true;

    for (std::map<std::string,int>::const_iterator it = parameter.begin(); it != parameter.end(); ++it)
    {
        std::string s = it->first;
        if (it->second != DM::SYSTEM)
            continue;
        
        std::vector<DM::View> views = this->views[s];
        //Check Reads
        if (DataValidation::isVectorOfViewRead(views))
        {
            //If the internal counter is > 0 data could be need for back link!
            if (this->internalCounter == 0)
                this->data_vals[s] = 0;
            //Default links are not fulfilled
            Port* inPort = this->getInPort(s);
            Port* outPort = this->getOutPort(s);

            inPort->setFullyLinked(false);
            if (outPort != 0)
                outPort->setFullyLinked(false);

            DM::System * sys = this->getSystemData(s);
            if (sys == 0)
                continue;

            //check for reads data. For read access only no new system state is created.
            foreach (DM::View view,  views)
            {
                if ( !view.reads() )
                    continue;

                //Check if all Data for read are really avalible

                //Get View saved in System
                DM::View * checkView = sys->getViewDefinition(view.getName());
                if (!checkView) 
				{
                    DM::Logger(DM::Warning) << "View Definition not found: " << this->getName()<< " " << view.getName();
                    sys = 0;
                    break;
                }
                //Check Type. Since all components can be used as component it can be used "downwards"
                if (checkView->getType() != view.getType() && view.getType() != DM::COMPONENT) {
                    DM::Logger(DM::Warning) << "Couldn't validate View " << view.getName() << " type difference";
                    sys = 0;
                    break;
                }
                //Get DummyComponent
                //DM::Component *c = checkView->getDummyComponent();
				Component* c = sys->getComponent(checkView->getDummyComponent()->getUUID());
				checkView->setDummyComponent(c);
				//Check if attributes are avalible
                if (c == 0){
                    sys = 0;
                    DM::Logger(DM::Warning) << "Dummy component does not exist in view: " << checkView->getName();
                    break;
                }
                /*if (c->getCurrentSystem() != sys){
                    sys = 0;
                    DM::Logger(DM::Warning) << "Dummy component not found in system: "
                                            << c->getQUUID().toString().toStdString();
                    break;
                }*/
                //Check if attributes to read are avalible in component
                foreach (std::string a, view.getReadAttributes())
                {
                    std::map<std::string, DM::Attribute*> existing_attributes = c->getAllAttributes();
                    if(!map_contains(&existing_attributes, a))
                    {
                        sys = 0;
                        DM::Logger(DM::Warning) << "Couldn't valideate View " << view.getName() << "Attribute missing" << a;
                        break;
                    }
                }
                if (sys == 0)
                    break;
            }
            if (sys == 0)
                continue;
            //All Checks successful -> update data
            this->data_vals[s] = sys;

            //this->getInPort(s)->setFullyLinked(true);
            this->_isFullyLinked = true;
            inPort->setFullyLinked(true);
            if (outPort)
                outPort->setFullyLinked(true);
        }

        //Creats a new Dataset for a complet new dataset
        if (!DataValidation::isVectorOfViewRead(views)) {
            this->data_vals[s] = this->getSystem_Write(s, views);
            this->getOutPort(s)->setFullyLinked(true);
        }
        else	//Create new system state for data that get modified
        {
            DM::System * sys_old = this->data_vals[s];
            if (sys_old != 0)
                foreach (DM::View v, views)
                    sys_old->addView(v);
        }
        this->data_vals[s]->setAccessedByModule(this);
    }
	/*
    foreach(PortObserver * po, this->portobserver) {
        if (!po)	is bullshit ^^
            po->changedPorts();*/
}

bool Module::checkIfAllSystemsAreSet() 
{
    for (std::map<std::string,int>::const_iterator it = parameter.begin(); it != parameter.end(); ++it)
    {
        if (it->second != DM::SYSTEM)
            continue;
        std::string name = it->first;
        if(!map_contains(&data_vals, name) || this->data_vals[name] == 0) {
            Logger(Error) << name << " " << " not set for module " << this->getUuid() << " " << this->getName();
            this->simulation->setSimulationStatus(SIM_ERROR_SYSTEM_NOT_SET);
            return false;
        }
    }
    return true;
}

void Module::setParameterValue(std::string name, std::string v) 
{
    this->setExecuted(false);
    //Check if parameter exists
    if(!map_contains(&parameter, name))
        Logger(Error) << "Parameter " << name << "does not exist";
    else
    {
        void* ref = NULL;
        if(map_contains(&parameter_vals, name, ref))
            convertValues(ref, parameter[name], QString::fromStdString(v));
    }
}


void Module::postRun() 
{
    this->internalCounter++;
    //To make sure that a module gets the right data when used in backlinks

    if (!_isFullyLinked)
        data_vals.clear();
    this->data_vals_prev = data_vals;
    //delete prev data
    deep_delete(&ownedSystems_prev);
    ownedSystems_prev = ownedSystems;
    ownedSystems.clear();
}


std::string Module::getParameterAsString(std::string Name) 
{
    std::stringstream ss;
    ss.precision(16);

    switch(parameter[Name])
    {
    case DM::DOUBLE:	ss << this->getParameter<double>(Name);
        return ss.str();
    case DM::INT:		ss << this->getParameter<int>(Name);
        return ss.str();
    case DM::BOOL:		ss << this->getParameter<bool>(Name);
        return ss.str();
    case DM::LONG:		ss << this->getParameter<long>(Name);
        return ss.str();
    case DM::STRING:
    case DM::FILENAME:	ss <<  this->getParameter<std::string>(Name);
        return ss.str();
    case DM::STRING_LIST:
    {
        std::vector<std::string> vec = this->getParameter<std::vector<std::string> >(Name);
        foreach (std::string s, vec)
            ss << s << "*|*" ;
        return ss.str();
    }
    case DM::STRING_MAP:
    {
        std::map<std::string, std::string> map = this->getParameter<std::map<std::string, std::string> >(Name);
        for (std::map<std::string, std::string>::iterator it = map.begin(); it != map.end(); ++it)
            ss << it->first << "*|*" << it->second << "*||*" ;
        return ss.str();
    }
    }
    return ss.str();
}
void Module::addData(std::string name,  std::vector<DM::View> views) {
    if (views.size() == 0)
    {
        Logger(Error) << "No views added data not created";
        return;
    }
    this->data_vals[name] = 0;
    //Only add to parameterList if not already in vector parameterList
    if(!vector_contains(&parameterList, name))
        this->parameterList.push_back(name);
    this->views[name] = views;
    this->parameter[name] = DM::SYSTEM;
    //Add Ports
    if (DM::DataValidation::isVectorOfViewRead(views))
        this->addPort(name, DM::INSYSTEM);
    if (DM::DataValidation::isVectorOfViewWrite(views))
        this->addPort(name, DM::OUTSYSTEM);
}

void Module::removeData(string dataToRemove)
{
    if(!vector_contains(&parameterList, dataToRemove))
        return;
    this->parameter_vals.erase(dataToRemove);
    this->views.erase(dataToRemove);
    this->parameter.erase(dataToRemove);
    this->data_vals.erase(dataToRemove);
    this->parameterList.erase(find(parameterList.begin(), parameterList.end(), dataToRemove));

    this->removePort(dataToRemove, DM::INSYSTEM);
    this->removePort(dataToRemove, DM::OUTSYSTEM);
}

DM::System* Module::getData(std::string dataname)
{
    if(!map_contains(&data_vals, dataname))
    {
        Logger(Debug) << "No System " << dataname;
        return 0;
    }
    else if(!map_contains(&data_vals,dataname))
    {
        Logger(Debug) << "No System " << dataname;
        return 0;
    }

    if (!this->_isFullyLinked) {
        Logger(Debug) << "Module not fully linked return sys 0";
        return 0;
    }

    return data_vals[dataname];
}

std::map<std::string, std::vector<DM::View> > Module::getViews() 
{
    return this->views;
}

DM::RasterData* Module::getRasterData(string dataname, const  DM::View & v) {

    DM::System * sys = data_vals[dataname];
    if (v.getAccessType() == DM::WRITE)
        return sys->addRasterData(new RasterData(), v);

    std::map<std::string, DM::Component*> components = sys->getAllComponentsInView(v);
    mforeach(Component* c, components)
            if(c->getType() == DM::RASTERDATA)
            return (RasterData*)c;

    Logger(Error) << "RasterData " << dataname << " doesn't exists";
    this->getSimulation()->setSimulationStatus(DM::SIM_ERROR_SYSTEM_NOT_SET);
    return 0;
}

void Module::addParameter(std::string name,int type, void * ref, std::string description) 
{
    this->parameter[name] = type;
    this->parameter_vals[name] = ref;
    this->parameterList.push_back(name);
    this->parameter_description[name] = description;
}

void Module::Destructor() 
{
    if (this->group != 0)
    {
        deep_delete(&InPorts);
        deep_delete(&OutPorts);

        this->group->removeModule(this);
        this->group = 0;
    }
    deep_delete(&ownedSystems);
}

void Module::convertValues(void * out, int Type, QString in) 
{
    if(!out)
        return;

    switch(Type)
    {
    case DM::LONG:
        *(long*)out = in.toLong();
        return;
    case DM::INT:
        *(int*)out = in.toInt();
        return;
    case DM::BOOL:
        *(bool*)out = in.toInt();
        return;
    case DM::DOUBLE:
        *(double*)out = in.toDouble();
        return;
    case DM::STRING:
    case DM::FILENAME:
        *(std::string*)out = in.toStdString();
        return;
    case DM::STRING_LIST:
    {
        ((std::vector<std::string>*)out)->clear();
        QStringList list = in.split("*|*");
        foreach(QString s, list)
            if (! s.isEmpty())
                ((std::vector<std::string>*)out)->push_back(s.toStdString());
    }
        return;
    case DM::STRING_MAP:
    {
        ((std::map<std::string, std::string>*)out)->clear();
        QStringList list = in.split("*||*");
        foreach(QString s, list)
        {
            if (! s.isEmpty())
            {
                QStringList list2 = s.split("*|*");
                ((std::map<std::string, std::string>*)out)->insert(
                            std::pair<std::string,std::string>(list2[0].toStdString(),list2[1].toStdString()) );
            }
        }
    }
        return;
    }
    return;
}
void Module::removePort(std::string LinkedDataName, int PortType) 
{
    Logger(Debug) << "RemovePort" << LinkedDataName;
    std::vector<Port*>* ports = (PortType < DM::OUTPORTS) ? &OutPorts : &InPorts;

    for (std::vector<Port * >::iterator it = ports->begin(); it != ports->end(); ++it)
    {
        if((*it)->getLinkedDataName() == LinkedDataName)
        {
            Logger(Debug) << "Delete Port" << LinkedDataName;
            delete *it;
            ports->erase(it);
            break;
        }
    }
    foreach(PortObserver * po, this->portobserver)
        po->changedPorts();
}
void Module::addPort(std::string LinkedDataName, int PortType) 
{
    Logger(Debug) << "AddPort" << LinkedDataName;

    foreach (Port *p_existing, (PortType < DM::OUTPORTS)?getOutPorts():getInPorts())
    {
        if (p_existing->getLinkedDataName() == LinkedDataName)
            return;
    }
    Port *p = new Port(this, PortType, LinkedDataName);
    if(PortType < DM::OUTPORTS)	OutPorts.push_back(p);
    else						InPorts.push_back(p);

    foreach(PortObserver * po, this->portobserver)
        po->changedPorts();
}

std::vector<Port*> Module::getInPorts() const {
    return this->InPorts;
}
std::vector<Port*> Module::getOutPorts() const {
    return this->OutPorts;
}

int Module::getParameterType(string Name)
{
    if (this->parameter.find(Name) == this->parameter.end()) {
        Logger(Error) << "No Parameter with this name";
        return -1;
    }
    return this->parameter[Name];
}

void Module::init() {

}

const DM::System* Module::getConstSystemData(const std::string &name)
{
    Port * p = this->getInPort(name);
    if (!p || p->getLinks().size() == 0)
        return 0;

    int LinkId = -1;
    int BackId = -1;
    int counter = 0;
    //Identify Positions in the Link Vector
    foreach (ModuleLink * l, p->getLinks())
    {
        if (!l->isBackLink())	LinkId = counter++;
        else					BackId = counter++;
    }
    if (LinkId < 0)
        return 0;

    ModuleLink * l = p->getLinks()[LinkId];

    if (this->internalCounter > 0 && BackId != -1)
    {
        l = p->getLinks()[BackId];
        Logger(Debug) << "BackLink for " << name;
        Logger(Debug) << "BackLink for " << l->getInPort()->getLinkedDataName();
    }
    Module * m = this->simulation->getModuleWithUUID(l->getUuidFromOutPort());
    //Counter Number of Links at Outport
    return  m->getSystemState(l->getDataNameFromOutPort());
}


DM::System* Module::getSystemData(const std::string &name)  
{
    Port * p = this->getInPort(name);
    if (!p || p->getLinks().size() == 0)
        return 0;

    int LinkId = -1;
    int BackId = -1;
    int counter = 0;
    int CounterBackLink = 0;
    //Identify Positions in the Link Vector
    foreach (ModuleLink * l, p->getLinks())
    {
        if (!l->isBackLink())
            LinkId = counter++;
        else
        {
            BackId = counter++;
            CounterBackLink++;
        }
    }
    if (LinkId < 0)
        return 0;

    ModuleLink * l = p->getLinks()[LinkId];

    if (this->internalCounter > 0 && BackId != -1)
    {
        l = p->getLinks()[BackId];
        Logger(Debug) << "BackLink for " << name;
        Logger(Debug) << "BackLink for " << l->getInPort()->getLinkedDataName();
    }
    Module * m = this->simulation->getModuleWithUUID(l->getUuidFromOutPort());
    //Counter Number of Links at Outport
    DM::System * returnSys =  m->getSystemState(l->getDataNameFromOutPort());
    if (!returnSys)
        return 0;

    if (m->getGroup() != this->getGroup())
    {
        Logger(Debug) << "Create successor for module outside group " << l->getInPort()->getLinkedDataName();
        return returnSys->createSuccessor();
    }
    DM::Port * out_p = m->getOutPort(l->getDataNameFromOutPort());
    // more than 1 standard link?
    if (out_p->getLinks().size() - CounterBackLink > 1)
    {
        Logger(Debug) << "Create for Split " << l->getInPort()->getLinkedDataName();
        return returnSys->createSuccessor();
    }
    if (isDebugMode())
    {
        Logger(Debug) << "Debug Split " << l->getInPort()->getLinkedDataName();
        return returnSys->createSuccessor();
    }
    return returnSys;
}


DM::System* Module::getSystemState(const std::string &name)
{
    // may use map_contains
    DM::System  * sys = this->data_vals_prev[name];

    if (sys == 0)
        return 0;

    if (!_isFullyLinked) {
        Logger(Debug) << "System is not fully linked return sys 0";
        return 0;

    }
    if (!_isFullyLinked) {
        Logger(Debug) << "System is not fully linked return sys 0";
        return 0;
    }
    sys->setAccessedByModule(this);
    return sys;
}


DM::System*   Module::getSystem_Write(std::string name, std::vector<DM::View> views)  
{
    DM::System * sys = NULL;
    if(!map_contains(&ownedSystems, name, sys))
        this->ownedSystems[name] = sys = new DM::System();

    if (!sys)
        return 0;

    foreach (DM::View v, views)
        sys->addView(v);

    sys->addView(DM::View ("dummy", DM::SUBSYSTEM, DM::WRITE));

    return sys;
}

void Module::setID(const int id) {
    this->id = id;
}

int Module::getID() const {
    return id;
}

void Module::setGroup(Group *group) 
{
    if(this->group)
        this->group->removeModule(this);
    this->group = group;
    this->group->addModule(this);
}
Group * Module::getGroup() const {
    return this->group;
}
void Module::setSimulation(Simulation *simulation) {
    this->simulation = simulation;
}

void Module::copyParameterFromOtherModule(Module * m) {
    std::string name_origin = m->getClassName();
    std::string name_this = this->getClassName();
    this->uuid = m->getUuid();
    this->debugMode = m->isDebugMode();
    //if (name_origin.compare(name_this) == 0) {
    if(name_origin == name_this)
    {
        std::map<std::string, int> parameterList = m->getParameterList();
        for ( std::map<std::string, int>::iterator it = parameterList.begin(); it != parameterList.end(); ++it)
            if (it->second < DM::USER_DEFINED_INPUT)
                this->setParameterValue(it->first, m->getParameterAsString(it->first));

        //Set Group
        this->setGroup(m->getGroup());
        this->name = m->getName();
        foreach(PortObserver * p, m->getPortObserver())
            this->portobserver.push_back(p);

        foreach(ResultObserver * p, m->getResultObserver())
            this->resultobserver.push_back(p);
    }
    else
        Logger(Error) << "Can't Copy Model Parameter from different Type of Module";
}
bool Module::isExecuted() {
    return this->hasBeenExecuted;
}

void Module::setExecuted(bool ex)
{
    Logger(Debug) << "SetExecuted " << ex << this->getClassName() << " " << this->getUuid();
    this->hasBeenExecuted = ex;

    if(!ex && this->getGroup())
        this->getGroup()->setContentOfModuleHasChanged(true);
}


std::string Module::getHelpUrl() {
    return "";
}

void Module::setDebugMode(bool mode) {
    this->debugMode = mode;
}

bool Module::isDebugMode() {
    return debugMode;
}

bool Module::isFullyLinked()
{
    return this->_isFullyLinked;
}
}
