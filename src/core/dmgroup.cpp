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
#include "dmgroup.h"
#include <dmport.h>
#include <dmmodulelink.h>
#include <dmporttuple.h>
#include <time.h>
#include <QStringList>
#include <QThreadPool>
#include <dmmodulerunnable.h>
#include <QMutexLocker>
#include <QRunnable>
#include <QMutex>
#include <dmporttuple.h>
#include <dmportobserver.h>
#include <dmrootgroup.h>
#include <QThreadPool>
#include <algorithm>
#include <sstream>
#include <dmsimulation.h>

namespace DM {
Group::Group()
{
    //mutex = new QMutex(QMutex::Recursive );
    moduleHasChanged = true;
    step = 0;
    Steps = 0;
}
void Group::addModule(Module *module) {
    if(module)
        this->modules.push_back(module);
}
Group::~Group() 
{
    this->Destructor();
    Module::Destructor();
    //delete mutex;
}

void Group::setContentOfModuleHasChanged(bool c)
{
    this->moduleHasChanged = c;
}
void Group::Destructor() 
{
    while(this->modules.size() > 0)
        delete *(this->modules.begin());
	deep_delete(&inPortTuple);
	deep_delete(&outPortTuple);
/*
    foreach(PortTuple * p, this->inPortTuple)
        delete p;

    foreach(PortTuple * p, this->outPortTuple)
        delete p;

    this->inPortTuple.clear();
    this->outPortTuple.clear();*/

}
void Group::finishedModule(Module *m) {
    //QMutexLocker locker(mutex);
    currentRunning.erase(std::find(currentRunning.begin(), currentRunning.end(), m));
	if(!m->isGroup() || ((Group*)m)->isRunnable())
	{
		UsedModules.push_back(m);
        notUsedModules.erase(std::find(notUsedModules.begin(), notUsedModules.end(), m));
		if(m->isGroup())
		{
            Group* g = (Group*)m;
			//Don't change status if virtualRun
            if (!g->getSimulation()->isVirtualRun()) 
			{
                g->setExecuted(true);
                g->setContentOfModuleHasChanged(false);
            }
            g->resetSteps();
		}
	}

	/*
    bool remove = true;
    if (m->isGroup()) {
        Group * g = (Group *) m;
        if (g->isRunnable()) {
            remove = false;
        }
    }


    if (remove) {
        UsedModules.push_back(m);
        notUsedModules.erase(std::find(notUsedModules.begin(), notUsedModules.end(), m));
        if (m->isGroup()) {
            Group * g = (Group * )m;
            //Don't change status if virtualRun
            if (!g->getSimulation()->isVirtualRun()) {
                g->setExecuted(true);
                g->setContentOfModuleHasChanged(false);
            }
            g->resetSteps();
        }
    }
	*/
    QVector<QRunnable * > modules = this->getNextJobs();
    foreach (QRunnable * r, modules)
        DMRootGroup::getThreadPool()->start(r);
}
void Group::clearModules() {
    this->modules.clear();
}

void Group::resetModules() 
{
    foreach(Module * m, this->modules) 
	{
        if (!m->isExecuted() || m->isGroup())
		{
            m->resetParameter();
            m->setExecuted(false);
            if (m->isGroup()) 
			{
                Group* g = (Group*)m;
                g->setContentOfModuleHasChanged(true);
                g->resetSteps();
            }
        }
    }
}

void Group::removeModule(Module *module) 
{
	for (std::vector<Module * >::iterator it = this->modules.begin(); it != this->modules.end();++it) 
	{
		if (*it == module) 
		{
			it = this->modules.erase(it);
			return;
		}
	}
}
PortTuple * Group::getInPortTuple(std::string name) const
{
	foreach(PortTuple* pt, inPortTuple)
		if(pt->getName() == name)
			return pt;
    /*for (std::vector<PortTuple*>::iterator it = this->inPortTuple.begin(); it != this->inPortTuple.end(); ++it) {
        PortTuple * p = *it;
        if (p->getName().compare(name) == 0)
            return p;
    }*/
    return 0;
}
PortTuple * Group::getOutPortTuple(std::string name) const
{
	foreach(PortTuple* pt, inPortTuple)
		if(pt->getName() == name)
			return pt;
	/*for (std::vector<PortTuple*>::iterator it = this->outPortTuple.begin(); it != this->outPortTuple.end(); ++it) {
        PortTuple * p = *it;

        if (p->getName().compare(name) == 0)
            return p;
    }*/
    return 0;
}
Port* Group::getPort(std::string name) const
{
	PortTuple *p = NULL;
	if(p = getInPortTuple(name))
		return p->getInPort();	
	else if(p = getOutPortTuple(name))
		return p->getOutPort();
	/*foreach(PortTuple *pt, outPortTuple)
		if(pt->getName() == name)
			return pt->getOutPort();

	foreach(PortTuple *pt, inPortTuple)
		if(pt->getName() == name)
			return pt->getInPort();*/
	return 0;
}

Port * Group::getInPort( std::string name) const
{
	if(Port *p = getPort(name))
		return p;

    return Module::getInPort(name);
}
Port * Group::getOutPort(std::string name) const
{
	if(Port *p = getPort(name))
		return p;

    return Module::getOutPort(name);
}


PortTuple * Group::addTuplePort(std::string LinkedDataName, int PortType) {
    PortTuple * pt = 0;
    Logger(Debug) << "Add Tuple Port" << LinkedDataName;

    if (PortType < DM::OUTPORTS) {
        //Check if port with the same name already exists
        pt = new PortTuple(this, LinkedDataName, PortType, false);
        foreach (PortTuple *pt_existing, outPortTuple) {
            if (pt_existing->getName().compare(pt->getName()) == 0)
                return 0;
        }
        this->outPortTuple.push_back(pt);
    } else {
        pt = new PortTuple(this, LinkedDataName, PortType, true);
        foreach (PortTuple *pt_existing, inPortTuple) {
            if (pt_existing->getName().compare(pt->getName()) == 0)
                return 0;
        }
        this->inPortTuple.push_back(pt);
    }
    foreach(PortObserver * po, this->portobserver) {
        po->changedPorts();
    }
    return pt;
}

void Group::removeTuplePort(PortTuple *pt)
{
    Logger(Debug) << "Remove Tuple Port" << pt->getName();
    int PortType = pt->getPortType();
    if (PortType < DM::OUTPORTS) {
        std::vector<PortTuple*>::iterator pt_it = find(outPortTuple.begin(), outPortTuple.end(), pt);
        if (pt_it == outPortTuple.end() )
            return;
        this->outPortTuple.erase(pt_it);
    } else {
        std::vector<PortTuple*>::iterator pt_it = find(inPortTuple.begin(), inPortTuple.end(), pt);
        if (pt_it == inPortTuple.end() )
            return;
        this->inPortTuple.erase(pt_it);
    }
    foreach(PortObserver * po, this->portobserver) {
        po->changedPorts();
    }
}

QVector<QRunnable *>  Group::getNextJobs() {
    QVector<QRunnable * > RunnedModulesInStep;
    for (std::vector<Module *>::iterator it = notUsedModules.begin(); it != notUsedModules.end(); ++it) {
        Module * m = *it;
        if (std::find(currentRunning.begin(), currentRunning.end(), m) !=  currentRunning.end())
            continue;

        bool runnable = true;
        std::vector<Port*> inPorts = m->getInPorts();
        if (m->isGroup()) {
            Group * g = (Group*) m;
            foreach(PortTuple * pt , g->getInPortTuples()) {
                inPorts.push_back(pt->getInPort());
            }
        }
        foreach(Port * p, inPorts) {
            foreach( ModuleLink * l, p->getLinks() ) {
                if (!l->isBackLink()) {
                    Module * neededM = l->getOutPort()->getModule();
                    bool ModuleExists = false;
                    foreach(Module* usedM, UsedModules) {
                        if (usedM == neededM) {
                            if (!neededM->isExecuted())
                                m->setExecuted(false);
                            ModuleExists = true;
                            break;
                        }
                    }
                    if (!ModuleExists) {
                        runnable = false;
                        break;
                    }
                }
            }
        }
        if (runnable) {
            currentRunning.push_back(m);
            RunnedModulesInStep.push_back(new ModuleRunnable(m));
        }

    }
    //Run Parallelisation TODO
    if(!this->notUsedModules.size()) {
        if (this->group != 0) {
            this->run();
        }

    }
    return RunnedModulesInStep;
}
void Group::resetSteps()
{
    this->step = 0;
    foreach (DM::Module * m, this->modules) {
        m->setInternalCounter(0);
    }


}

void Group::run() {
    if (this->step ==0) {
        if (!this->isExecuted())
            this->resetModules();
    }

    notUsedModules  = this->modules;
    if (notUsedModules.size() == 0) {
        this->step = Steps;
    }
    UsedModules.clear();
    currentRunning.clear();
    UsedModules.push_back(this);
    if (this->isRunnable()) {
        //QMutexLocker locker(mutex);
        QVector<QRunnable * > modules = this->getNextJobs();
        foreach (QRunnable * r, modules) {
            if (!r) {
                Logger(Error) << "Null Pointer";
                return;

            }
            DMRootGroup::getThreadPool()->start(r);
        }

        if (this->getSimulation()->isVirtualRun() || this->moduleHasChanged == false)
            this->step = Steps;

        this->step++;

        if (this->isRunnable()) {
            foreach (DM::Module * m, this->modules) {
                m->setExecuted(false);
            }
        }
    } else {
        this->group->finishedModule(this);
    }


}
void Group::setExecuted(bool ex) {
    Module::setExecuted(ex);
    if (!ex)
        this->resetSteps();
}

}
