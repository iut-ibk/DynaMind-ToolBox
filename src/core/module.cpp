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


using namespace std;
namespace vibens {
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
    //this->addParameter("InputDouble", VIBe2::USER_DEFINED_DOUBLEDATA_IN, &InputDoubleData);


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

void Module::updateParameter() {
    for (boost::unordered_map<std::string,int>::const_iterator it = parameter.begin(); it != parameter.end(); ++it) {
        std::string s = it->first;

        if (it->second == VIBe2::SYSTEM) {
            DM::System ** v = (DM::System**) this->parameter_vals[s];
            std::vector<DM::View> views= this->views[s];
            bool reads = false;
            bool writes = false;
            foreach (DM::View view,  views)  {
                if (reads == true)
                    continue;
                reads = view.reads();
            }
            foreach (DM::View view,  views)  {
                if (writes == true)
                    continue;
                writes = view.writes();
            }

            if (reads) {
                *v = &this->getSystemData(s);
            }
            //Creats a new Dataset
            if (writes && !reads) {
                *v = &this->getSystem_Write(s);
            }

        }

        if (it->second == VIBe2::SYSTEM_IN) {
            DM::System ** v = (DM::System**) this->parameter_vals[s];
            *v = &this->getSystemData(s);
        }
        if (it->second == VIBe2::SYSTEM_OUT) {
            DM::System ** v = (DM::System**) this->parameter_vals[s];
            *v = &this->getSystem_Write(s);
        }

        if (it->second == VIBe2::DOUBLEDATA_IN) {
            double * d = (double*) this->parameter_vals[s];
            *d = this->getDoubleData(s);
        }





        /*if (it->second == VIBe2::USER_DEFINED_VECTORDATA_IN) {
            //OPTIMIZAZION POSSIBLE
            std::map<std::string, VectorData*> * ref = (std::map<std::string, VectorData*> *)this->parameter_vals[s];
            std::map<std::string, VectorData*>  ref_new;
            for (std::map<std::string, VectorData*>::const_iterator it = ref->begin(); it != ref->end(); ++it) {
                std::string name = it->first;
                ref_new.insert(std::pair<std::string, VectorData*>(name,& this->getVectorData(name)));
            }
            *ref = ref_new;
        }*/



        QString ss = "DoubleIn_" + QString::fromStdString(s);
        for (std::map<std::string, double>::iterator it1 = this->InputDoubleData.begin(); it1 != InputDoubleData.end(); ++it1 ){

            if (ss.toStdString().compare(it1->first) == 0) {
                if (parameter[s] == VIBe2::LONG) {
                    long * d = (long*) this->parameter_vals[s];
                    *d = (long) this->getDoubleData(ss.toStdString());
                }
                if (parameter[s] == VIBe2::INT) {
                    int * d = (int*) this->parameter_vals[s];
                    *d = (int) this->getDoubleData(ss.toStdString());
                    Logger(Debug) << "Singel Value From OutSide" << s <<" INT" << *d;
                }
                if (parameter[s] == VIBe2::DOUBLE) {
                    double * d = (double*) this->parameter_vals[s];
                    *d = (double) this->getDoubleData(ss.toStdString());
                    Logger(Debug) << "Singel Value From OutSide" << s << *d;
                }
                //Logger(Debug) << "Singel Value From OutSide" << s << *d;

            }
        }
        if (it->second == VIBe2::USER_DEFINED_DOUBLEDATA_IN) {
            //this->parameter_vals[s] = this->getDoubleData(name);

            std::map<std::string, double> & dmap = *((std::map<std::string, double> *) this->parameter_vals[s]);
            if (&dmap != &InputDoubleData) { // already done in DoubleIn lines see above

                for (std::map<std::string, double>::const_iterator it = dmap.begin(); it != dmap.end(); ++it) {
                    std::string name = it->first;
                    dmap[name] = this->getDoubleData(name);
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
    if(parameter[name] == VIBe2::LONG) {
        long * ref = (long * )this->parameter_vals[name];
        if (ref == 0)
            return;
        *ref =  value.toLong();
        return;
    }
    if(parameter[name] == VIBe2::INT) {
        int * ref = (int * )this->parameter_vals[name];
        if (ref == 0)
            return;
        *ref =  value.toInt();
        return;
    }
    if(parameter[name] == VIBe2::BOOL) {
        bool * ref = (bool * )this->parameter_vals[name];
        if (ref == 0)
            return;
        *ref =  (bool) value.toInt();
        return;
    }
    if(parameter[name] == VIBe2::DOUBLE) {
        double * ref = (double * )this->parameter_vals[name];
        if (ref == 0)
            return;
        *ref =  value.toDouble();
        return;
    }
    if(parameter[name] == VIBe2::STRING || parameter[name] == VIBe2::FILENAME) {
        std::string * ref = (std::string * )this->parameter_vals[name];
        if (ref == 0)
            return;
        *ref =  value.toStdString();
        return;
    }
    if (parameter[name] == VIBe2::USER_DEFINED_DOUBLEDATA_IN) {
        std::map<std::string, double> * ref = (std::map<std::string, double> *)this->parameter_vals[name];
        if (ref == 0)
            return;
        QStringList list = value.split("*|*");
        foreach(QString s, list) {
            if (! s.isEmpty()) {
                ref->insert( std::pair<std::string,double>(s.toStdString(),0) );
                std::stringstream ss;
                ss << s.toStdString();
                this->addPort(ss.str(), VIBe2::INDOUBLEDATA);
            }
        }
        return;
    }
    /*if (parameter[name] == VIBe2::USER_DEFINED_VECTORDATA_IN) {
        std::map<std::string, VectorData*> * ref = (std::map<std::string, VectorData*> *)this->parameter_vals[name];
        if (ref == 0)
            return;
        QStringList list = value.split("*|*");
        foreach(QString s, list) {
            if (! s.isEmpty()) {
                ref->insert( std::pair<std::string,VectorData*>(s.toStdString(),0) );
                std::stringstream ss;
                ss << s.toStdString();
                this->addPort(ss.str(), VIBe2::INVECTOR);
            }
        }
        return;
    }*/
    if (parameter[name] == VIBe2::STRING_MAP) {
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
void Module::removeFromUserDefinedParameter(std::string name, std::string v) {

    if (parameter[name] == VIBe2::USER_DEFINED_DOUBLEDATA_IN ) {
        std::map<std::string, double> * ref = (std::map<std::string, double> *)this->parameter_vals[name];
        std::map<std::string, double>::iterator element_to_erase = ref->find(v);
        ref->erase(element_to_erase);
        this->removePort(v, VIBe2::INDOUBLEDATA);
    }


}

void Module::appendToUserDefinedParameter(std::string name, std::string  v){

    QString value = QString::fromStdString(v);
    if (parameter[name] == VIBe2::USER_DEFINED_DOUBLEDATA_IN) {
        std::map<std::string, double> * ref = (std::map<std::string, double> *)this->parameter_vals[name];
        QStringList list = value.split("*|*");
        foreach(QString s, list) {
            if (! s.isEmpty()) {
                ref->insert( std::pair<std::string,double>(s.toStdString(),0) );
                std::stringstream ss;
                ss << s.toStdString();
                this->addPort(ss.str(), VIBe2::INDOUBLEDATA);
            }
        }
        return;
    }


}

void Module::setParameter() {
    this->internalCounter++;
    for (boost::unordered_map<std::string,int>::const_iterator it = parameter.begin(); it != parameter.end(); ++it) {
        if (it->second == VIBe2::DOUBLEDATA_OUT) {
            double val;
            std::string s = it->first;
            val = this->getParameter<double>(s);
            this->setDoubleData(s, val);
        }
    }

}


std::string Module::getParameterAsString(std::string Name) {
    int ID = this->parameter[Name];
    std::stringstream ss;
    ss.precision(16);
    if (ID == VIBe2::DOUBLE)
        ss << this->getParameter<double>(Name);
    if (ID == VIBe2::INT)
        ss << this->getParameter<int>(Name);
    if (ID == VIBe2::BOOL)
        ss << this->getParameter<bool>(Name);
    if (ID == VIBe2::STRING || ID  == VIBe2::FILENAME)
        ss << this->getParameter<std::string>(Name);
    if (ID == VIBe2::LONG)
        ss << this->getParameter<long>(Name);

    if (ID == VIBe2::USER_DEFINED_DOUBLEDATA_IN) {
        std::map<std::string, double> map = this->getParameter<std::map<std::string, double> >(Name);
        for (std::map<std::string, double>::iterator it = map.begin(); it != map.end(); ++it) {
            ss << it->first << "*|*";
        }
    }
    if (ID == VIBe2::STRING_MAP) {
        std::map<std::string, std::string> map = this->getParameter<std::map<std::string, std::string> >(Name);
        for (std::map<std::string, std::string>::iterator it = map.begin(); it != map.end(); ++it) {
            ss << it->first << "*|*" << it->second << "*||*" ;
        }

    }

    return ss.str();
}


void Module::addData(std::string name,  std::vector<DM::View> views, void * ref) {
    this->parameter_vals[name] = ref;
    this->parameterList.push_back(name);
    this->views[name] = views;
    this->parameter[name] = VIBe2::SYSTEM;

    bool reads = false;
    bool writes = false;
    foreach (DM::View view,  views)  {
        if (reads == true)
            continue;
        reads = view.reads();
    }
    foreach (DM::View view,  views)  {
        if (writes == true)
            continue;
        writes = view.writes();
    }
    if (reads) {
        this->addPort(name, VIBe2::INSYSTEM);
    }
    if (writes) {
        this->addPort(name, VIBe2::OUTSYSTEM);
    }
}

void Module::addParameter(std::string name,int type, void * ref, std::string description) {

    this->parameter[name] = type;
    this->parameter_vals[name] = ref;
    this->parameterList.push_back(name);
    this->parameter_description[name] = description;

    if (type == VIBe2::DOUBLEDATA_OUT) {
        this->addPort(name, VIBe2::OUTDOUBLEDATA);
        this->createDoubleData(name);
    }
    if (type == VIBe2::DOUBLEDATA_IN) {
        this->addPort(name, VIBe2::INDOUBLEDATA);
    }

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

    if (Type == VIBe2::INT) {
        int * v = (int*) value;
        *(v) =  val.toInt();
        return;
    }
    if (Type == VIBe2::LONG) {
        long * v = (long*) value;
        *(v) =  val.toLong();
        return;
    }
    if (Type == VIBe2::DOUBLE) {
        double * v = (double*) value;
        *(v) =  val.toDouble();
        return;
    }
    if (Type== VIBe2::STRING || Type== VIBe2::FILENAME) {
        std::string * v = (std::string *) value;
        *(v) =  val.toStdString();
        return;
    }

    if (Type== VIBe2::USER_DEFINED_DOUBLEDATA_IN) {
        std::map<std::string, double> * map =  (std::map<std::string, double> *) value;
        QStringList list = val.split(QRegExp("\\s+"));
        foreach(QString s, list) {
            if (! s.isEmpty()) {
                map->insert(std::pair<std::string, double>(s.toStdString(), 0));
            }
        }
    }
    return;
}
void Module::removePort(std::string LinkedDataName, int PortType) {
    Logger(Debug) << "RemovePort" << LinkedDataName;

    if (PortType < VIBe2::OUTPORTS) {
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
    if (PortType < VIBe2::OUTPORTS) {
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

void Module::init(const parameter_type &parameters) {

    init_called = true;
    paramRaw = parameters;

    for (boost::unordered_map<std::string, int>::const_iterator it = parameter.begin(); it != parameter.end(); ++it) {
        std::string s = it->first;
        if (parameters.find(s) != parameters.end()) {
            QString val = QString::fromStdString(parameters.at(s));
            this->convertValus(this->parameter_vals[s], parameter[s], val);
        }
    }

}


DM::System   &Module::getSystemData(const std::string &name)  {
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
        Logger(Debug) << "BackLink for " << name;
    }
    Logger(Debug) << "BackLink for " << l->getInPort()->getLinkedDataName();


    Module * m = this->simulation->getModuleWithUUID(l->getUuidFromOutPort());
    return m->getSystemState(l->getDataNameFromOutPort());
    //return simulation->getDataBase()->getRasterData(l->getUuidFromOutPort(), l->getDataNameFromOutPort(), true, l->isBackLinkInChain());

}

DM::System & Module::getSystemState(const std::string &name) {



    DM::System  * sys= *(DM::System**) this->parameter_vals[name];



    return *sys->createSuccessor();

}
DM::System   &Module::getSystem_Write(const std::string &name)  {

    DM::System * sys = new DM::System(name, name);
    //sys->addView(this->views[name]);

    return *sys;

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

        ro->addRasterDataToViewer(r);
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

int Module::getID() {
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
            if (it->second < VIBe2::USER_DEFINED_INPUT) {
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
