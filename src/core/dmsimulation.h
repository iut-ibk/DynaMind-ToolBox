/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @author  Markus Sengthaler <m.sengthaler@gmail.com>
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

#ifndef DMSIMULATION_H
#define DMSIMULATION_H

#include "dmcompilersettings.h"
#include <string>
#include <vector>
//#include <QThread>
//#include <dmrootgroup.h>
#include <dmmodule.h>
#include <dmsystem.h>


namespace DM {
struct SimulationPrivate;
class DataObserver;
class SimulationObserver;
class Module;
class Port;
class IDataBase;
class ModuleRegistry;
class ModuleLink;
class Group;

static const  std::string CoreVersion = DYNAMIND_VERSION_CORE;

enum SimulationStatus {
    SIM_OK,
    SIM_FAILED_LOAD,
    SIM_ERROR_SYSTEM_NOT_SET,
	SIM_FAILED,

};

/**
* @class DM::Simulation
*
*
* @ingroup DynaMind-Core
*
* @brief Here is where all the playground is set up. Modules and groups get wired to simulations and
* executed.
*
* Example Code
*
* dmsimulatlion.h
* @code
* #include <ostream>
* #include <dmlog.h>
* #include <dmsimulation.h>
* #include <dmmodule.h>
* //Before a simulation can be executed we need to intialise the Logger
*
* void main()
* {
*      ostream *out = &cout;
*      //Define LogLevel (DM::Debug, DM::Standard, DM::Warning, DM::Error)
*      DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
*      DM::Simulation sim;
*      //Register the modules (load them from the shared library dynamind-testmodules)
*      sim.registerNativeModules("dynamind-testmodules");
*      //Add new module
*      DM::Module * m = sim.addModule("TestModule");
*      //Add another module
*      DM::Module * inout  = sim.addModule("InOut");
*      //link the two module together
*      DM::ModuleLink * l = sim.addLink(m->getOutPort("Sewer"), inout->getInPort("Inport"));
*      //execute the simulation
*      sim.run();
* }
* @endcode
*/

class DM_HELPER_DLL_EXPORT Simulation
{
public:
	class Link
	{
	public:
		Module* src;
		std::string outPort;

		Module* dest;
		std::string inPort;

		bool isIntoGroupLink;
		bool isOutOfGroupLink;

		/** @brief shortcut to src data */
		System* getData()
		{
			if(src->isGroup() && dest->getOwner() == src)
				// into group link
				return src->getInPortData(outPort);
			else
				return src->getOutPortData(outPort);
		}
		/** @brief shifts data from source to destination */
		void shiftData(bool successor = false)
		{
			System * data = getData();
			if(!data)
				return;
			if(successor)
				data = data->createSuccessor();
			// shift pointer
			if(dest->isGroup() && src->getOwner() == dest)
				// out of group link
				dest->setOutPortData(inPort, data);
			else
			{
				dest->setInPortData(inPort, data);
				// FIX: modules which won't call getData(...)  
				// won't get data on the out port
				if(dest->hasOutPort(inPort) && !dest->isGroup())
					dest->setOutPortData(inPort, data);
			}
		}
	};
	Simulation();
	~Simulation();
	/** @brief adds a module to the simulation, returning a pointer to the object. returns 0 if failed. */
    virtual Module* addModule(const std::string ModuleName, Module* parent = NULL, bool callInit = true);
    /** @brief Removes and deletes a module from the simulation */
    virtual void removeModule(Module* m);
	/** @brief register a new native module returns if module has been loaded succcessfully */
    bool registerModule(const std::string& filepath);
	/** @brief registers all acceptable modules in the provided directory*/
	void registerModulesFromDirectory(const QDir& dir);
	/** @brief connects to ports via a link */
	virtual bool addLink(Module* source, std::string outPort, Module* dest, std::string inPort, bool checkStream = true);
    /** @brief removes a link */
	bool removeLink(Module* source, std::string outPort, Module* dest, std::string inPort);
    /** @brief starts the entire simulation */
	void run();
    /** @brief Resets the whole simulation */
    void reset();
    /** @brief Resets the whole simulation, deprecated, for backwards compatibility */
	void resetSimulation() {reset();};
	/** @brief after a Simulation is executed this parameter returns if something happend in between the simulation */
	SimulationStatus getSimulationStatus() {return status;};
	/** @brief accessor to module registry */
	ModuleRegistry* getModuleRegistry(){return moduleRegistry;};
	/** @brief loads modules from default locations */
	void registerModulesFromDefaultLocation();
	/** @brief Add the modules set in the QSetting **/
    bool registerModulesFromSettings();
	
    /** @brief adds a simulation saved in a file to the current simulation */
    virtual bool loadSimulation(std::string filePath);
    /** @brief writes the simulation to a xml file */
    virtual void writeSimulation(std::string filename);

	void clear();
	
	std::list<Module*> getModules(){return modules;};
	std::list<Link*> getLinks(){return links;};

	//QFuture<void> decoupledRun();
	void cancel();
	bool isLinkingValid(Module* source, std::string outPort, Module* dest, std::string inPort, bool logOutput);
	void addObserver(SimulationObserver *obs);
	void removeObserver(SimulationObserver *obs);

	bool loadSimulation(QIODevice* source, QString filepath, 
		std::map<std::string, DM::Module*>& modMap, 
		DM::Module* overwrittenOwner = NULL, bool overwriteGroupOwner = false);
	void writeSimulation(QIODevice* dest, QString filePath);
	
private:
	/** @brief shifts data from the outgoing port of a module to the inport of the successor module
		returns destination module */
	std::set<Module*> shiftModuleOutput(Module* m);
	std::set<Module*> shiftGroupInput(Group* m);

	std::list<Module*>	modules;
	std::list<Link*>	links;

	SimulationStatus	status;
	ModuleRegistry*		moduleRegistry;
	
	/** @brief checks the stream for possible missing views */
	bool checkStream();
	bool checkModuleStreamForward(Module* m);
	//bool checkModuleStreamForward(Module* m, std::string streamName);
	bool checkModuleStream(Link* link);
	bool checkGroupStreamForward(Group* g, std::string streamName, bool into);
	bool canceled;
	
	std::vector<Link*> getIngoingLinks(const Module* dest, const std::string& inPort) const;
	std::vector<Link*> getOutgoingLinks(const Module* src, const std::string& outPort) const;
	std::vector<Link*> getIntoGroupLinks(const Module* src, const std::string& inPort) const;
	std::vector<Link*> getOutOfGroupLinks(const Module* dest, const std::string& outPort) const;

	std::vector<SimulationObserver*>	observers;
};

}
#endif // SIMULATION_H
