/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @author  Markus Sengthaler <m.sengthaler@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2011-2012  Christian Urich, Markus Sengthaler

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
#include <queue>

#include "dmsimulation.h"
#include "dmsimulationreader.h"
#include "dmmodule.h"
#include "dmmoduleregistry.h"
#include "dmmoduleparameterreader.h"
#include "dmmoduleregistry.h"
#include "dmsimulationobserver.h"
#include <dmmodule.h>
//#include <dmmodulelink.h>
//#include <dmport.h>
#include <dmmoduleregistry.h>
#include <dmgroup.h>
#include <dmsimulationwriter.h>
#include <dmlogger.h>
//#include <dmporttuple.h>

#ifndef PYTHON_EMBEDDING_DISABLED
#include <dmpythonenv.h>
#endif

#include <QSettings>
#include <QDir>
//#include <QThread>

#include <QtConcurrentRun>


namespace DM {

Simulation::Simulation()
{
	status = SIM_OK;
	moduleRegistry = new ModuleRegistry();
}

Simulation::~Simulation()
{
	clear();
	// TODO: cleanup lost systems
	delete moduleRegistry;

	foreach(SimulationObserver* obs, observers)
		delete obs;
}

Module* Simulation::addModule(const std::string ModuleName, Module* parent, bool callInit)
{
	Module *module = this->moduleRegistry->createModule(ModuleName);
    if(!module)
        return NULL;

	module->setOwner(parent);

	modules.push_back(module);
    Logger(Debug) << "Added module" << ModuleName;
    if (callInit)
        module->init();

	return module;
}

void Simulation::removeModule(Module* m)
{
	// TODO check if systems are lost
	Logger(Debug) << "Removing module" << m->getName();
	
	std::vector<Link*> toDelete;

	foreach(Link* l, links)
		if(l->dest == m || l->src == m)
			toDelete.push_back(l);

	foreach(Link* l, toDelete)
	{
		links.remove(l);
		delete l;
	}

	modules.remove(m);
	delete m;
}

void Simulation::clear()
{
	foreach(Module* m, modules)
		removeModule(m);
}

bool Simulation::registerModule(const std::string& filepath) 
{
	//Logger(Standard) << "Loading native module " << filepath;

	QString qfilepath = QString::fromStdString(filepath);
	
	if(qfilepath.endsWith(".py"))
	{
		QFileInfo fi = qfilepath;
		DM::PythonEnv::getInstance()->addPythonPath(fi.absolutePath().toStdString());
		try
		{
			DM::PythonEnv::getInstance()->registerNodes(moduleRegistry, fi.fileName().remove(".py").toStdString());
			Logger(Debug) <<  "successfully loaded python module " << filepath;
			return true;
		}
		catch(...) 
		{
			Logger(Warning) <<  "failed loading python module " << filepath;
            return false;
		}
	}
	else//(qfilepath.endsWith(".dll") || qfilepath.endsWith(".so") || qfilepath.endsWith(".ko"))
	{
		if(moduleRegistry->addNativePlugin(filepath))
		{
			DM::Logger(Debug) <<  "successfully loaded native module " << filepath;
			return true;
		}
		else
		{
			Logger(Warning) <<  "failed loading native module " << filepath;
			return false;
		}
	}
	//Logger(Warning) << "not recognized filename ending " << filepath;
	//return false;
}

void Simulation::registerModulesFromDirectory(const QDir& dir)
{
	QStringList modulesToLoad = dir.entryList();
	foreach (QString module, modulesToLoad) 
	{
		if (module == ".." || module == "." )
			continue;
		std::string modulepath = (dir.absolutePath() +"/" + module).toStdString();
		registerModule(modulepath);
	}
}

void Simulation::registerModulesFromDefaultLocation()
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

	foreach (QDir cp, cpv)  
		registerModulesFromDirectory(cp);
}

bool Simulation::isLinkingValid(Module* source, std::string outPort, Module* dest, std::string inPort, 
								bool logOutput)
{
	if(!source || !dest)
	{
		if(logOutput)	Logger(Error) << "invalid module (NULL pointer)";
		return false;
	}
	if(source == dest)
	{
		if(logOutput)	Logger(Error) << "cannot link module itself";
		return false;
	}

	// group stuff
	if(dest->owner == source)	// into group link
	{
		if(!source->hasInPort(outPort))
		{
			if(logOutput)	Logger(Error) << "accessing not existing inner group in-port";
			return false;
		}
	}
	else if(!source->hasOutPort(outPort))
	{
		if(logOutput)	Logger(Error) << "accessing not existing out-port";
		return false;
	}

	if(source->owner == dest)	// out of group link
	{
		if(!dest->hasOutPort(inPort))
		{
			if(logOutput)	Logger(Error) << "accessing not existing inner group out-port";
			return false;
		}
	}
	else if(!dest->hasInPort(inPort))
	{
		if(logOutput)	Logger(Error) << "accessing not existing in-port";
		return false;
	}

	// check if the same link exists already
	foreach(Link* l, links)
	{
		if(l->src == source && l->outPort == outPort && l->dest == dest && l->inPort == inPort)
		{
			if(logOutput)	Logger(Warning) << "Link already exists: " << outPort << "to" << inPort;
			return false;
		}
	}
	return true;
}

bool Simulation::addLink(Module* source, std::string outPort, Module* dest, std::string inPort, bool checkStream)
{
	if(!isLinkingValid(source, outPort, dest, inPort, true))
		return false;

	Link* l = new Link();
	l->src = source;
	l->outPort = outPort;
	l->dest = dest;
	l->inPort = inPort;
	l->isIntoGroupLink = (dest->owner == source);
	l->isOutOfGroupLink = (source->owner == dest);
	links.push_back(l);
	// stream check
	Logger(Debug) << "Added link from module '" << l->src->getClassName() << "' port '" << outPort 
							<< "' to module '" << l->dest->getClassName() << "' port '" << inPort << "'";

	if(checkStream)
	{
		if(checkModuleStream(l))
			Logger(Debug) << "checking stream successfull";
		else
			Logger(Warning) << "stream incomplete" ;
	}
	return true;
}
bool Simulation::removeLink(Module* source, std::string outPort, Module* dest, std::string inPort)
{
	Link* toDelete = NULL;
	foreach(Link* l, links)
	{
		if(	l->src == source && 
			l->outPort == outPort && 
			l->dest == dest && 
			l->inPort == inPort)
		{
			l->dest->reset();
			toDelete = l;
			break;
		}
	}
	if(toDelete)
	{
		links.remove(toDelete);
		Logger(Debug) << "Deleted link from port " << outPort << "to" << inPort;
		checkModuleStreamForward(dest, inPort);
		return true;
	}
	return false;
}

std::vector<Simulation::Link*> Simulation::getIngoingLinks(const Module* dest, const std::string& inPort) const
{
	std::vector<Simulation::Link*> ls;
	foreach(Link* l, links)
		if(	l->dest == dest && l->inPort == inPort && !l->isOutOfGroupLink)
			ls.push_back(l);
	
	return ls;
}
std::vector<Simulation::Link*> Simulation::getOutgoingLinks(const Module* src, const std::string& outPort) const
{
	std::vector<Simulation::Link*> ls;
	foreach(Link* l, links)
		if(	l->src == src && l->outPort == outPort && !l->isIntoGroupLink)
			ls.push_back(l);

	return ls;
}
std::vector<Simulation::Link*> Simulation::getIntoGroupLinks(const Module* src, const std::string& inPort) const
{
	std::vector<Simulation::Link*> ls;
	foreach(Link* l, links)
		if(	l->src == src && l->outPort == inPort && l->isIntoGroupLink)
			ls.push_back(l);
	
	return ls;
}
std::vector<Simulation::Link*> Simulation::getOutOfGroupLinks(const Module* dest, const std::string& outPort) const
{
	std::vector<Simulation::Link*> ls;
	foreach(Link* l, links)
		if(	l->dest == dest && l->inPort == outPort && l->isOutOfGroupLink)
			ls.push_back(l);

	
	return ls;
}
/*
Module* Simulation::getFormerModule(Module* dest, std::string inPort, std::string& outPort)
{
	Link* l = getIngoingLink(dest, inPort);
	if(l)
	{
		//if(l->isIntoGroupLink)
		//	return getFormerModule(l->src, l->outPort, outPort);
		//	l = getIngoingLink(l->src, l->outPort);
		//if(l->isOutOfGroupLink)
		//	l = getOutgoingLink(l->src, l->outPort);
		
		outPort = l->outPort;
		return l->src;
	}
	return NULL;
}

Module* Simulation::getNextModule(Module* src, std::string outPort, std::string& inPort)
{
	Link* l = getOutgoingLink(src, outPort);
	if(l)
	{
		//if(l->isIntoGroupLink)
		//	l = getIngoingLink(l->src, l->outPort);
		//if(l->isOutOfGroupLink)
		//	l = getOutgoingLink(l->dest, l->inPort);

		inPort = l->inPort;
		return l->dest;
	}
	return NULL;
}
*/


bool Simulation::checkGroupStreamForward(Group* g, std::string streamName, bool into)
{
	if(!g)
	{
		DM::Logger(DM::Error) << "critical error: group == NULL at checkGroupStreamForward";
		return false;
	}

	bool success = true;

	std::vector<Link*> nextLinks;
	std::map<std::string, DM::View>* curStreamViews;

	if(into)
	{
		nextLinks = getIntoGroupLinks(g, streamName);
		curStreamViews = &g->streamViews[streamName];
	}
	else
	{
		nextLinks = getOutgoingLinks(g, streamName);
		curStreamViews = &g->outStreamViews[streamName];
	}

	foreach(Link* l, nextLinks)
	{
		l->dest->streamViews[l->inPort] = *curStreamViews;
		if(!checkModuleStreamForward(l->dest, l->inPort))
			success = false;
	}

	return success;
}

bool IsWriteOnly(const std::map<std::string, View>& stream)
{
	mforeach(const View& v, stream)
		if(v.getAccessType() != DM::WRITE)
			return false;
	return true;
}

bool Simulation::checkModuleStreamForward(Module* m, std::string streamName)
{
	bool success = true;
	/*if(m->isGroup())
	{
		return checkGroupStream(m, streamName, backwards?OUTPORT:INPORT);
		if(back)
		{
			foreach(Link* l, getOutgoingLinks(m, streamName))
				if(!checkModuleStream(l->src, l->outPort, true, false))
					success = false;
			return success;
		}
		if(forward)
		{
			foreach(Link* l, getIngoingLinks(m, streamName))
				if(!checkModuleStream(l->src, l->outPort, true, false))
					success = false;
			return success;
		}
		else
		{
			DM::Logger(DM::Error) << "group check not allowed";
			return false;
		}
	}*/
	//if(strcmp(m->getClassName(), "Marker") == 0)
	//	int i=0;

	std::map<std::string, DM::View>* curStreamViews = &m->streamViews[streamName];
	// check if we are in the middle of an unchecked stream
	if(curStreamViews->size() == 0 && m->hasInPort(streamName))
	{
		/*
		// go back to origin
		std::vector<Link*> inLinks;
		//if(!outOfGroup)
			inLinks = getIngoingLinks(m, streamName);
		//else
		//	inLinks = getOutOfGroupLinks(m, streamName);

		if(inLinks.size() == 0)
		{*/
			m->setStatus(MOD_CHECK_ERROR);
			DM::Logger(DM::Warning) << "missing link to module '" << m->getClassName() << "' port '"<< streamName <<"'";
			success = false;
		/*}
		else
			foreach(Link* l, inLinks)
				if(!checkModuleStream(l->src, l->outPort, true))
					success = false;
	*/
		return success;
	}
	// update stream view info in module
	std::map<std::string,View> updatedStream = *curStreamViews;
	//*streamViews = formerViews;

	
	DM::Logger(DM::Debug) << "initializing module '" << m->getClassName() << "'";
	m->init();
	DM::Logger(DM::Debug) << "checking stream '" << streamName << "' in module '" << m->getClassName() << "'";

	mforeach(const View& v, m->accessedViews[streamName])
	{
		if(v.getName() == "dummy")	// TODO: this is ugly, horrible, terrible and awful
			continue;				// but for backwards compatibility its necessary

		const int a = v.getAccessType();
		if(a == READ || a == MODIFY)
		{
			// check if we can access the desired view
			if(!map_contains(curStreamViews, v.getName()))
			{
				DM::Logger(DM::Error) << "module '" << m->getClassName() 
					<< "' tries to access the nonexisting view '" << v.getName()
					<< "' from stream '" << streamName << "'";
				m->setStatus(MOD_CHECK_ERROR);
				success = false;
				continue;
			}
		}
		else if(a == WRITE)	// add new views
			updatedStream[v.getName()] = v;
	}

	if(!success)
		return success;
	else
		m->setStatus(MOD_CHECK_OK);

	// get all out ports, all assigned links and push forward
	std::vector<Link*> outLinks;
	foreach(std::string outPortName, m->getOutPortNames())
		foreach(Link* outLink, getOutgoingLinks(m, outPortName))
			outLinks.push_back(outLink);

	foreach(Link* l, outLinks)
	{
		std::map<std::string,View> outStream = updatedStream;
		if(l->outPort != streamName)
		{
			const std::map<std::string,View>& streamAccess = m->accessedViews[l->outPort];
			if(IsWriteOnly(streamAccess))
				outStream = m->accessedViews[l->outPort];
			else
				continue;
		}

		if(outStream.size() == 0)
			continue;

		if(!l->isOutOfGroupLink)
			l->dest->streamViews[l->inPort] = outStream;
		else
			((Group*)l->dest)->outStreamViews[l->inPort] = outStream;

		if(!l->dest->isGroup())
		{
			if(!checkModuleStreamForward(l->dest, l->inPort))
				success = false;
		}
		else
		{
			if(!checkGroupStreamForward((Group*)l->dest, l->inPort, !l->isOutOfGroupLink))
				success = false;
		}
	}
	return success;
}

bool Simulation::checkModuleStream(Link* link)
{
	if(link->src->isGroup())
		return checkGroupStreamForward((Group*)link->src, link->outPort, link->isIntoGroupLink);
	else
		return checkModuleStreamForward(link->src, link->outPort);
	/*std::map<std::string, DM::View>* curStreamViews = &l->src->streamViews[streamName];
	// check if we are in the middle of an unchecked stream
	if(curStreamViews->size() == 0 && m->getInPortNames().size() != 0)
	{

	}*/
}

bool Simulation::checkModuleStreamForward(Module* m)
{
	bool success = true;
	std::map<std::string, std::map<std::string,View>> accessedViews = m->getAccessedViews();
	if(accessedViews.size() != 0)
	{
		// iterate through all streams
		for(std::map<std::string, std::map<std::string,View>>::iterator it = accessedViews.begin();
			it != accessedViews.end(); ++it)
		{
			if(!checkModuleStreamForward(m, it->first))
				success = false;
		}
	}
	else
	{
		
		if(!m->isGroup())
		{
			foreach(std::string inPortName, m->getInPortNames())
				if(!checkModuleStreamForward(m, inPortName))
					success = false;
		}
		else
		{
			foreach(std::string inPortName, m->getInPortNames())
				if(!checkGroupStreamForward((Group*)m, inPortName, true))
					success = false;
		}
		

		DM::Logger(DM::Debug) << "initializing module '" << m->getClassName() << "'";
		m->init();
	}

	return success;
}

bool Simulation::checkStream()
{
	bool success = true;
	QList<QFuture<bool>*> results;
	foreach(Module* m, modules)
	{
		//std::vector<std::string> inPorts = m->getInPortNames();
		if(m->getInPortNames().size() == 0)
			foreach(std::string outPort, m->getOutPortNames())
				if(!checkModuleStreamForward(m, outPort))
					success = false;

		/*if(m->getInPortNames().size() == 0)
		{
			if(!checkModuleStreamForward(m))
				return false;
		}*/
	}
	/*
	foreach(QFuture<bool>* r, results)
	{
		r->waitForFinished();
		if(!r->result())
			success = false;
		delete r;
	}
	return success;*/
	return success;
}

void Simulation::run()
{
	canceled = false;
	// notify progress
	foreach(SimulationObserver* obs, observers)
		obs->update(0);

	QElapsedTimer simtimer;
	simtimer.start();

	Logger(Standard) << ">> checking simulation";
	if(!checkStream())
	{
		Logger(Error) << ">> checking simulation failed";
		this->status = SIM_FAILED;
		return;
	}
	Logger(Standard) << ">> checking simulation succeeded (took " << (long)simtimer.elapsed() << "ms)";
	simtimer.restart();
	Logger(Standard) << ">> starting simulation";
	// get modules with no imput - beginning modules list
	std::queue<Module*> worklist;
	foreach(Module* m, modules)
		if(m->inPortsSet())
			worklist.push(m);
	
	int cntModulesFinished = 0;

	// run modules
	// if not started decoupled, the state of the future is canceled, started and finished
	while(worklist.size() && !canceled)
	{
		// get first element
		Module* m = worklist.front();
		worklist.pop();

		if(!m->isGroup())
		{
			// execute module
			Logger(Standard) << "running module '" << m->getName() << "'";
			QElapsedTimer modTimer;
			modTimer.start();
			m->setStatus(MOD_EXECUTING);
			QtConcurrent::run(m, &Module::run).waitForFinished();

			// check for errors
			ModuleStatus merr = m->getStatus();
			if(m->getStatus() == MOD_EXECUTION_ERROR)
			{
				if(Module* owner = m->getOwner())
					owner->setStatus(MOD_EXECUTION_ERROR);

				Logger(Error) << "module '" << m->getName() << "' failed after " << (long)modTimer.elapsed() << "ms";
				this->status = DM::SIM_FAILED;
				return;
			}
			else
			{
				Logger(Standard)	<< "module '" << m->getName() << "' executed successfully (took " 
									<< (long)modTimer.elapsed() << "ms)";
				m->setStatus(MOD_EXECUTION_OK);

				// notify progress
				cntModulesFinished++;
				float progress = (float)cntModulesFinished/modules.size();
				foreach(SimulationObserver* obs, observers)
					obs->update(progress);
			}
			// shift data from out port to next inport
			foreach(Module* nextModule, shiftModuleOutput(m))
				worklist.push(nextModule);
		}
		else
		{
			Group* g = (Group*)m;
			Logger(Standard) << "running group '" << g->getName() << "'";
			
			// first run, reset condition
			if(g->getStatus() != MOD_EXECUTING)
				g->resetCondition();

			if(g->condition())
			{
				Logger(Standard) << "condition fulfilled for group '" << g->getName() << "'";
				// execute group
				g->setStatus(MOD_EXECUTING);
				// instead of m::run() we simply shift the data to the first internal module
				foreach(Module* nextModule, shiftGroupInput(g))
					worklist.push(nextModule);
			}
			else
			{
				Logger(Standard) << "finishing group '" << g->getName() << "'";
				// finish and shift data
				g->setStatus(MOD_EXECUTION_OK);
				foreach(Module* nextModule, shiftModuleOutput(g))
					worklist.push(nextModule);
			}
		}
		/*
		else if(m->getStatus() == MOD_EXECUTING)
		{
			Group* g = (Group*)m;
			// we reached the end of the group
			std::vector<std::string> backLinks = g->loopBack();
			if(backLinks.size()>0)
			{
				Logger(Standard) << "rewinding group '" << g->getName() << "'";
				foreach(Module* nextModule, shiftGroupLoopData(g, backLinks))
					worklist.push(nextModule);
			}
			else
			{
				// finish and shift data
				g->setStatus(MOD_EXECUTION_OK);
				foreach(Module* nextModule, shiftModuleOutput(g))
					worklist.push(nextModule);
			}
		}
		else
		{
			Group* g = (Group*)m;
			// execute group
			Logger(Standard) << "running group '" << g->getName() << "'";
			g->setStatus(MOD_EXECUTING);
			// instead of m::run() we simply shift the data to the first internal module
			foreach(Module* nextModule, shiftGroupInput(g))
				worklist.push(nextModule);
		}*/
	}
	if(canceled)
	{
		Logger(Standard) << ">> canceled simulation (time elapsed " << (long)simtimer.elapsed() << "ms)";
		Logger(Debug) << ">> cleaning up";
		reset();
		// notify progress
		foreach(SimulationObserver* obs, observers)
			obs->update(0);
	}
	else
	{
		Logger(Standard) << ">> finished simulation (took " << (long)simtimer.elapsed() << "ms)";
		// notify progress
		foreach(SimulationObserver* obs, observers)
			obs->update(1);
	}

	//finished = true;
}
/*
class ThreadWrapper: public QThread
{
	Simulation* sim;
public:
	ThreadWrapper(Simulation* sim): sim(sim){}
	void run()
	{
		sim->run();
	}
};*/

/*
QFuture<void> Simulation::decoupledRun()
{


	//if(QThreadPool::globalInstance()->maxThreadCount() < 1)
	//	QThreadPool::globalInstance()->setMaxThreadCount(1);

	if(decoupledRunResult.isRunning() || runningModuleResult.isRunning())
		cancel();

	decoupledRunResult = QtConcurrent::run(this, &Simulation::run);
	return decoupledRunResult;
}*/

void Simulation::cancel()
{
	Logger(Standard) << ">> canceling simulation";
	/*runningModuleResult.cancel();
	decoupledRunResult.cancel();
	Logger(Standard) << ">> canceling simulation - waiting currently running modules to finish";
	runningModuleResult.waitForFinished();
	decoupledRunResult.waitForFinished();
	Logger(Standard) << ">> canceling simulation - finished";*/
	canceled = true;
}

std::list<Module*> Simulation::shiftModuleOutput(Module* m)
{
	std::list<Module*> nextModules;
	//mforeach(System* sys, m->outPorts)
	for(std::map<std::string, System*>::iterator it = m->outPorts.begin();
		it != m->outPorts.end();	++it)
	{
		// first get all links starting at the given module
		std::list<Link*> branches;
		std::vector<Link*> outLinks = getOutgoingLinks(m, it->first);
		foreach(Link* l, outLinks)
			if(l->getData())
				branches.push_back(l);

		/*foreach(Link* l, links)
			if(l->src == m && l->outPort == it->first && l->getData())	// check for assigned and existing data
				branches.push_back(l);*/

		if(branches.size() > 0)
		{
			bool createSuccessor = branches.size() > 1 || m->isSuccessorMode();
			foreach(Link* l, branches)
			{
				l->shiftData(createSuccessor);
				if(l->dest->inPortsSet())
					nextModules.push_back(l->dest);
			}
			// reset out port
			if(!createSuccessor)
				it->second = NULL;
		}
		else if(outLinks.size())
			Logger(Warning) << "module '" << m->getClassName() << "' does not write data to outport '" << it->first << "'";
		else
			// dead path
			Logger(Warning) << "outport '" << it->first << "' from module '" << m->getClassName() << "' not connected";
	}
	// reset in port
	for(std::map<std::string, System*>::iterator it = m->inPorts.begin();
		it != m->inPorts.end();	++it)
	{
		it->second = NULL;
	}

	return nextModules;
}

std::list<Module*> Simulation::shiftGroupInput(Group* g)
{
	std::list<Module*> nextModules;
	std::vector<std::string> inPorts = g->getInPortNames();
	foreach(std::string inPort, inPorts)
	{
		if(g->getInPortData(inPort))
		{
			foreach(Link* l, getIntoGroupLinks(g, inPort))
			{
				l->shiftData(inPorts.size() > 1);
				nextModules.push_back(l->dest);
			}
		}
	}
	return nextModules;
}

/*
std::list<Module*> Simulation::shiftGroupLoopData(Group* g, const std::vector<std::string>& backLinks)
{
	foreach(std::string portName, backLinks)
	{
		g->setInPortData(portName, g->getOutPortData(portName));
		g->setOutPortData(portName, NULL);
	}
	return shiftGroupInput(g);
}*/

void Simulation::reset()
{
    Logger(Standard) << ">> Reset Simulation";
	foreach(Module* m, this->modules)
	{
		m->reset();
		m->setStatus(MOD_UNTOUCHED);
	}
	checkStream();
}


bool Simulation::registerModulesFromSettings() 
{
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
	foreach (QString s, list)
		registerModulesFromDirectory(s);
#endif

	// Native Modules
	text = settings.value("nativeModules").toString();
	list = text.replace("\\","/").split(",");
	foreach (QString s, list) {
		if (s.isEmpty())
			continue;
		registerModule(s.toStdString());
	}

	return true;
}


// for old versions
void LoopGroupAdaptor(	QVector<LinkEntry>& links, 
						QVector<ModuleEntry>& modules, 
						ModuleEntry& loopGroup)
{
	// check if it is an old version
	QMap<QString, QString>::iterator itInView = loopGroup.parameters.find("nameOfInViews");
	QMap<QString, QString>::iterator itOutView = loopGroup.parameters.find("nameOfOutViews");

	if(itInView == loopGroup.parameters.end() || itOutView == loopGroup.parameters.end())
		return;

	// get modules in group for searching backlinks
	QMap<QString, ModuleEntry> inGroupModules;
	foreach(const ModuleEntry& m, modules)
		if(m.GroupUUID == loopGroup.UUID)
			inGroupModules[m.UUID] = m;

	// get backlinks
	QList<LinkEntry> backLinks;
	foreach(const LinkEntry& l, links)
		if(l.backlink && inGroupModules.find(l.InPort.UUID) != inGroupModules.end())
			backLinks.push_back(l);

	// remove backlinks
	

	foreach(const LinkEntry& bl, backLinks)
	{
		int index = -1;
		int i = 0;
		foreach(const LinkEntry& l, links)
		{
			if(		l.InPort.PortName == bl.InPort.PortName 
				&&	l.InPort.UUID == bl.InPort.UUID
				&&	l.OutPort.PortName == bl.OutPort.PortName
				&&	l.OutPort.UUID == bl.OutPort.UUID)
				index = i;
			{
				break;
				i++;
			}
		}
		if(index>0)
			links.remove(index); // TODO check
	}

	QList<QString> nameOfInPorts = itInView.value().split("*|*", QString::SkipEmptyParts);
	QList<QString> nameOfOutPorts = itOutView.value().split("*|*", QString::SkipEmptyParts);

	// remove old params
	loopGroup.parameters.remove("nameOfInViews");
	loopGroup.parameters.remove("nameOfOutViews");

	// add write streams
	QString writeStreams;
	foreach(const LinkEntry& backLink, backLinks)
	{
		// get link into group, connecting loop entry with backlink end
		QString inPortName;
		QString outPortName;
		foreach(const LinkEntry& refl, links)
		{
			if(refl.InPort.PortName == backLink.InPort.PortName
				&& refl.InPort.UUID == backLink.InPort.UUID
				&& refl.OutPort.UUID == loopGroup.UUID)
			{
					inPortName = refl.OutPort.PortName;
			}
		}
		foreach(const LinkEntry& refl, links)
		{
			if(refl.OutPort.PortName == backLink.OutPort.PortName
				&& refl.OutPort.UUID == backLink.OutPort.UUID
				&& refl.InPort.UUID == loopGroup.UUID)
			{
					outPortName = refl.InPort.PortName;
			}
		}
		if(inPortName.size() == 0)	// cant resolve link
		{
			DM::Logger(Error) << "Backlink could not be converted from old loopgroup format";
			return;
		}
		// add new write stream
		writeStreams += inPortName + "*|*";
		// remove from in ports, as the port is added with the write stream parameter
		nameOfInPorts.removeAll(inPortName);
		nameOfOutPorts.removeAll(outPortName);
		// reconnect links, as the outport from the loopgroup has been renamed
		// from 'backLink.OutPort.PortName' to 'portName'
		for(QVector<LinkEntry>::iterator innerLink = links.begin(); innerLink != links.end(); ++innerLink)
		{
			if(innerLink->OutPort.PortName == backLink.OutPort.PortName
				&& innerLink->OutPort.UUID == backLink.OutPort.UUID
				&& innerLink->InPort.UUID == loopGroup.UUID)
			{
				// this link goes to an outport
				// change all outgoing links on the outside
				for(QVector<LinkEntry>::iterator outerLink = links.begin(); outerLink != links.end(); ++outerLink)
				{
					if(outerLink->OutPort.UUID == innerLink->InPort.UUID
						&& outerLink->OutPort.PortName == innerLink->InPort.PortName)
					{
						outerLink->OutPort.PortName = inPortName;
					}
				}
				innerLink->InPort.PortName = inPortName;
			}
		}
	}
	// add read streams
	QString readStreams;
	foreach(QString portName, nameOfInPorts)
		readStreams += portName + "*|*";


	// TRICKY: we assume that if out ports are left, one read stream is meant to be a write stream
	// that must fulfill following conditions:
	// just a single out and a single in port, no write streams so far
	if(nameOfOutPorts.size() == 1 && writeStreams.size() == 0 && nameOfInPorts.size() == 1)
	{
		// get names
		QString oldOutPortName = nameOfOutPorts.front();
		QString newOutPortName = nameOfInPorts.at(0);
		// exchange
		writeStreams = readStreams;
		readStreams.clear();
		// relink
		for(QVector<LinkEntry>::iterator it = links.begin(); it != links.end(); ++it)
		{
			if(it->OutPort.UUID == loopGroup.UUID && it->OutPort.PortName == oldOutPortName)
				it->OutPort.PortName = newOutPortName;
			else if(it->InPort.UUID == loopGroup.UUID && it->InPort.PortName == oldOutPortName)
				it->InPort.PortName = newOutPortName;
		}
		DM::Logger(DM::Debug) << "exchanged port '" << oldOutPortName << "' of module '" 
			<< loopGroup.ClassName << "' with port '" << newOutPortName << "'";
	}
	
	loopGroup.parameters["writeStreams"] = writeStreams;
	loopGroup.parameters["readStreams"] = readStreams;
}

void UpdateVersion(QVector<LinkEntry>& links, QVector<ModuleEntry>& modules)
{
	for(QVector<ModuleEntry>::iterator it = modules.begin(); it != modules.end(); ++it)
		if(it->ClassName == "LoopGroup")
			LoopGroupAdaptor(links, modules, *it);
}

bool Simulation::_loadSimulation(std::string filename, std::map<std::string, DM::Module*>& modMap) 
{
	QDir simFileDir = QFileInfo(QString::fromStdString(filename)).absoluteDir();	// for param corr.
	Logger(Standard) << ">> loading simulation file '" << filename << "'";
    SimulationReader simreader(QString::fromStdString(filename));
	
	QVector<ModuleEntry> moduleEntries = simreader.getModules();
	QVector<LinkEntry> linkEntries = simreader.getLinks();
	UpdateVersion(linkEntries, moduleEntries);

	int waitingForGroup = 0;
	// load modules
	while(moduleEntries.size() > 0 && moduleEntries.size() >= waitingForGroup)
	//foreach(ModuleEntry me, simreader.getModules())
	{
		ModuleEntry me = moduleEntries.first();
		moduleEntries.pop_front();

		DM::Module* owner = modMap[me.GroupUUID.toStdString()];
		if(me.GroupUUID.size() && me.GroupUUID != simreader.getRootGroupUUID())
		{
			if(!owner)
			{
				moduleEntries.push_back(me);
				waitingForGroup++;
				continue;
			}
			else
				waitingForGroup--;
		}
		// do not init module - we first have to set parameters and links as well as checking the stream!
		if(DM::Module* m = addModule(me.ClassName.toStdString(), owner, false))
		{
			modMap[me.UUID.toStdString()] = m;
			// load parameters
			for(QMap<QString, QString>::iterator it = me.parameters.begin(); it != me.parameters.end(); ++it)
			{
				std::string value = it.value().toStdString();
				if(DM::Module::Parameter* p = m->getParameter(it.key().toStdString()))
				{
					if(p->type == DM::FILENAME)
					{
						QDir path(simFileDir.absoluteFilePath(it.value()));
						DM::Logger(DM::Debug) << "converted relative path '" << value << "'";
						value = path.canonicalPath().toStdString();
						DM::Logger(DM::Debug) << "to absolute path '" << value << "'";
					}
				}


				m->setParameterValue(it.key().toStdString(), value);
			}
			// we init now (probably two times) to init e.g. ports
			m->init();
		}
		else
			DM::Logger(Error) << "creating module '" << me.ClassName.toStdString() << "' failed";
	}
	if(waitingForGroup>0)
		foreach(ModuleEntry me, moduleEntries)
			DM::Logger(Error) << "could not find the group for module '" << me.ClassName.toStdString() << "'";

	// load links
	foreach(LinkEntry le, linkEntries)
	{
		DM::Module *src = modMap[le.OutPort.UUID.toStdString()];
		DM::Module *dest = modMap[le.InPort.UUID.toStdString()];
		std::string outPort = le.OutPort.PortName.toStdString();
		std::string inPort = le.InPort.PortName.toStdString();

		if(!src || !dest)
			DM::Logger(Error) << "corrupt link";
		else
		{
			if(!addLink(src, outPort, dest, inPort, false))
				DM::Logger(Error) << "could not establish link between "
				<< src->getClassName() << ":" << outPort << " and "
				<< dest->getClassName() << ":" << inPort;
		}
	}
	Logger(Standard) << ">> checking stream";
	if(checkStream())
		Logger(Standard) << ">> checking stream finished successfully";
	else
		Logger(Error) << ">> error checking stream";
	
	/*Logger(Standard) << ">> initializing modules";
	foreach(DM::Module* m, modules)
		m->init();*/

	Logger(Standard) << ">> loading simulation file finished";
	return true;
}

bool Simulation::loadSimulation(std::string filename) 
{
	std::map<std::string, DM::Module*> modMap;
	return _loadSimulation(filename, modMap);
}

void Simulation::writeSimulation(std::string filename) 
{
    SimulationWriter::writeSimulation(filename, this);
}

void Simulation::addObserver(SimulationObserver *obs)
{
	observers.push_back(obs);
}
void Simulation::removeObserver(SimulationObserver *obs)
{
	std::vector<SimulationObserver*>::iterator it = find(observers.begin(), observers.end(), obs);
	if(it != observers.end())
		observers.erase(it);
}




#ifdef OLD_WF
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




Module * Simulation::addModule(std::string ModuleName, bool callInit) {

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
    if (callInit)
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
#ifndef PYTHON_EMBEDDING_DISABLED
    //Init Python
    QSettings settings;
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
#endif
}
void Simulation::loadModulesFromDefaultLocation()
{
    QVector<QDir> cpv;
    cpv.push_back(QDir(QDir::currentPath() + "/Modules"));
    cpv.push_back(QDir(QDir::currentPath() + "/bin/Modules"));
#ifdef _DEBUG
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
        std::cout <<  cp.absolutePath().toStdString() << std::endl;
        foreach (QString module, modulesToLoad) 
		{
            if (module == ".." || module == ".")
                continue;
            DM::Logger(DM::Debug) << module.toStdString();
            std::cout <<  module.toStdString() << std::endl;
            QString ml = cp.absolutePath() +"/" + module;
            if (this->moduleRegistry->addNativePlugin(ml.toStdString()))
                loadedModuleFiles.push_back(ml.toStdString());
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
void Simulation::loadPythonModulesFromDirectory(std::string path) {
#ifndef PYTHON_EMBEDDING_DISABLED
    QDir pythonDir = QDir(QString::fromStdString(path));
    QStringList filters;
    filters << "*.py";
    QStringList files = pythonDir.entryList(filters);
    DM::PythonEnv::getInstance()->addPythonPath((path));
    foreach(QString file, files) 
	{
        try{
            std::string n = DM::PythonEnv::getInstance()->registerNodes(moduleRegistry, file.remove(".py").toStdString());
            loadedModuleFiles.push_back(file.toStdString());
        } catch(...) {
            Logger(Warning)  << "Can't load Module " << file.toStdString();

        }
    }
#endif
}

bool Simulation::addModulesFromSettings() {
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
        Logger(Standard) << "Loading Native Modules " <<s.toStdString();
        if (moduleRegistry->addNativePlugin(s.toStdString())) {
            loadedModuleFiles.push_back(s.toStdString());
        }
    }

    return true;
}

Simulation::Simulation() 
{
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

bool Simulation::registerNativeModules(string Filename) 
{
    Logger(Standard) << "Loading Native Modules " << Filename ;
    return moduleRegistry->addNativePlugin(Filename);
}

void Simulation::registerPythonModules(std::string path) 
{
#ifndef PYTHON_EMBEDDING_DISABLED
    DM::PythonEnv::getInstance()->addPythonPath(path);
    QDir pythonDir = QDir(QString::fromStdString(path));
    QStringList filters;
    filters << "*.py";
    QStringList files = pythonDir.entryList(filters);
    foreach(QString file, files) 
	{
        Logger(Debug) << "Loading Python module: " << file.remove(".py").toStdString();
        std::string n = DM::PythonEnv::getInstance()->registerNodes(moduleRegistry, file.remove(".py").toStdString());
    }
#endif
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

    foreach(Module * m, this->getModules())
        if (m->isGroup())
            groups.push_back((Group*)m);

    return groups;
}

void Simulation::resetModules() 
{
    std::vector<DM::Module *> mv = this->getModules();
    foreach (Module * m, mv) 
	{
        if (!m->isExecuted()) 
		{
            //Make sure all data are deleted
            m->resetParameter();
            this->resetModule(m->getUuid());
        }
    }
}

void Simulation::resetSimulation()
{
    std::vector<DM::Module *> mv= this->getModules();
    foreach (Module * m, mv) 
	{
        m->resetParameter();
        this->resetModule(m->getUuid());
    }
}
void Simulation::run() 
{
    this->resetModules();
    this->startSimulation(false);
}

bool Simulation::startSimulation(bool virtualRun) 
{
    if (!virtualRun)
        this->startSimulation(true);
    this->data->simulationStatus = SIM_OK;
    this->virtualRun = virtualRun;
    Logger(Standard) << "Run Simulation";
    Logger(Debug) << "Reset Steps";
    this->rootGroup->resetSteps();
    Logger(Debug) << "Start Simulations";
    if (this->rootGroup->getModules().size() > 0)
        this->rootGroup->run();

    if (!virtualRun) 
	{
        Logger(Standard) << "End Simulation";
        return true;
    }
    //Execute Simulation Observer
    foreach (SimulationObserver * so, data->simObserver)
        so->VirtualRunDone();

    return true;
}

void Simulation::removeModule(std::string UUid) 
{
	delete_element(&Modules, UUid);
}
void Simulation::deregisterModule(std::string UUID) {

    //if(!this)
    //    return;
	remove_element(&Modules, UUID);
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

std::map<std::string, std::string>  Simulation::loadSimulation(std::string filename) 
{
    std::map<std::string, std::string> UUIDTranslator;
    SimulationReader simreader(QString::fromStdString(filename));

    foreach (ModuleEntry me, simreader.getModules()) 
	{
        Module * m = this->addModule(me.ClassName.toStdString(), false);
        if (!m) 
		{
            this->setSimulationStatus(SIM_FAILED_LOAD);
            return std::map<std::string, std::string>();
        }

        m->setName(me.Name.toStdString());
        m->setDebugMode(me.DebugMode);
        UUIDTranslator[me.UUID.toStdString()] = m->getUuid();
        foreach(QString s, me.ParemterList.keys())
            m->setParameterValue(s.toStdString(), me.ParemterList[s].toStdString());
    }
    //Reconstruct Groups;
    foreach (ModuleEntry me, simreader.getModules()) 
	{
        if (me.GroupUUID != simreader.getRootGroupUUID()) 
		{
            Module * m = this->getModuleWithUUID(UUIDTranslator[me.UUID.toStdString()]);
            Group * g = (Group *)this->getModuleWithUUID(UUIDTranslator[me.GroupUUID.toStdString()]);
            m->setGroup(g);
        }
    }
    //Call init Functions of the modules
	mforeach(Module* m, this->Modules)
		m->init();
    /*foreach (ModuleEntry me, simreader.getModules()) {
        Module * m = this->getModuleWithUUID(UUIDTranslator[me.UUID.toStdString()]);
        m->init();
    }*/

    foreach (LinkEntry le, simreader.getLinks()) 
	{
        std::string outPortUUID = UUIDTranslator[le.OutPort.UUID.toStdString()];
        std::string inPortUUID = UUIDTranslator[le.InPort.UUID.toStdString()];
        DM::Port * p_out = 0;
        DM::Port * p_in = 0;
		std::string outPortName = le.OutPort.PortName.toStdString();
		std::string inPortName = le.InPort.PortName.toStdString();

        if (!inPortUUID.empty() && !outPortUUID.empty() ) 
		{
            if (le.OutPort.isTuplePort == 1)
			{
                Group * g = (Group*) this->getModuleWithUUID(outPortUUID);
				PortTuple *pt = NULL;

                if (g->getUuid() != this->getModuleWithUUID(inPortUUID)->getGroup()->getUuid())
                    pt = g->getOutPortTuple(outPortName);
				else 
                    pt = g->getInPortTuple(outPortName);

				if (pt == 0) 
				{
                    Logger(Error) << "OutPorttuple " << le.OutPort.PortName.toStdString() <<" doesn't exist";
                    continue;
                }
                p_out = pt->getOutPort();
            }
			else 
                p_out = this->getModuleWithUUID(outPortUUID)->getOutPort(outPortName);
            
            if (le.InPort.isTuplePort == 1)
			{
                //Check if Connected Module is within this group
                Group * g = (Group*) this->getModuleWithUUID(inPortUUID);
                PortTuple *pt = NULL;

                if (g->getUuid() != this->getModuleWithUUID(inPortUUID)->getGroup()->getUuid())
					pt = g->getInPortTuple(inPortName);
				else
					pt = g->getOutPortTuple(inPortName);
                
				if (pt == 0) 
				{
					Logger(Error) << "InPorttuple " << le.InPort.PortName.toStdString() <<" doesn't exist";
					continue;
				}
				p_in = pt->getInPort();
            } 
			else
                p_in = this->getModuleWithUUID(inPortUUID)->getInPort(inPortName);

            ModuleLink * l = this->addLink(p_out, p_in);

            if (l != 0)
                l->setBackLink(le.backlink);
        }
    }
    return UUIDTranslator;
}

Module * Simulation::resetModule(std::string UUID) 
{
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
			Port *pIn = new_m->getInPort(name);
			if(!pIn){
				Logger(Error) << "Cant find port " << name << " in module " << new_m->getClassName();
				continue;
			}
            l->setInPort(pIn);
            p->removeLink(l);
        }

    }
    foreach(Port * p, m->getOutPorts()) {
        foreach(ModuleLink * l, p->getLinks()) {
            std::string name = l->getOutPort()->getLinkedDataName();
			Port* pOut = new_m->getOutPort(name);
			if(!pOut){
				Logger(Error) << "Cant find port " << name << " in module " << new_m->getClassName();
				continue;
			}
            l->setOutPort(pOut);
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
        foreach (Module * m, g->getModules())
            m->setGroup(new_g);

        g->clearModules();
    }
    delete m;
    Modules[new_m->getUuid()] = new_m;
    return new_m;
}
std::vector<ModuleLink*> Simulation::getLinks() 
{
    std::vector<ModuleLink*> links;
    foreach (Module * m, this->getModules()) 
	{
        foreach(Port * p, m->getInPorts())
            foreach(ModuleLink * l, p->getLinks())
                links.push_back(l);

        if (m->isGroup()) 
		{
            Group * g = (Group *) m;
            foreach (PortTuple *  pt, g->getInPortTuples()) 
			{
                Port * p = pt->getInPort();
                foreach(ModuleLink * l, p->getLinks())
                    links.push_back(l);
            }
            foreach (PortTuple *  pt, g->getOutPortTuples()) 
			{
                Port * p = pt->getInPort();
                foreach(ModuleLink * l, p->getLinks())
                    links.push_back(l);
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

    /*for (std::map<std::string, Module*>::const_iterator it = this->Modules.begin(); it != this->Modules.end(); ++it)
        ms.push_back(it->second);*/

	mforeach(Module* m, Modules)
		ms.push_back(m);
    return ms;
}
#endif



}


