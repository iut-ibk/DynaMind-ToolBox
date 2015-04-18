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
#include <fstream>
#include <vector>
#include <dmmodule.h>
#include <dmisystem.h>

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

class DM_HELPER_DLL_EXPORT Core
{
	public:
		static std::string getCoreVersion();
		static std::string getAuthor();
		static std::string getLicense();
		static std::string getContact();
};

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
* 
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
*      // Define LogLevel = {DM::Debug, DM::Standard, DM::Warning, DM::Error}
*      DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
*      DM::Simulation sim;
*      // Register the modules (load them from the shared library dynamind-testmodules)
*      sim.registerNativeModules("dynamind-testmodules");
*      // Add new module
*      DM::Module * m = sim.addModule("TestModule");
*      // Add another module
*      DM::Module * inout  = sim.addModule("InOut");
*      // link the two module together
*      sim.addLink(m, "Sewer", inout, "Inport");
*      // execute the simulation
*      sim.run();
* }
* @endcode
*/

struct DM_HELPER_DLL_EXPORT Link
{
	Module*		src;
	std::string outPort;
	Module*		dest;
	std::string inPort;

	bool isIntoGroupLink;
	bool isOutOfGroupLink;
};

class DM_HELPER_DLL_EXPORT SimulationConfig {
private:
	int CoordinateSystem; //As EPSG Code
	std::string workingDir;
	bool keepSystems;
public:
	SimulationConfig() : CoordinateSystem(0), workingDir(QString(QDir::tempPath()+"/dynamind").toStdString()), keepSystems(false){}
	void setCoordinateSystem(int epsgCode) {CoordinateSystem = epsgCode;}
	int getCoorindateSystem() const {return this->CoordinateSystem;}
	bool getKeepSystems() const;
	void setKeepSystems(bool value);
	std::string getWorkingDir() const;
	void setWorkingDir(const std::string &value);
};


class DM_HELPER_DLL_EXPORT Simulation
{
public:

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
	virtual bool addLink(Module* source, std::string outPort, 
						Module* dest, std::string inPort, bool checkStream = true);

	/** @brief removes a link */
	bool removeLink(Module* source, std::string outPort, Module* dest, std::string inPort);

	/** @brief starts the entire simulation */
	void run();

	/** @brief Resets the whole simulation */
	void reset();

	/** @brief Resets the whole simulation, deprecated, for backwards compatibility */
	void resetSimulation() {reset();};

	/** @brief Cancels thin simulation, waits till the currently running module finishes.
	All successing modules get skipped */
	void cancel();

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

	/** @brief removes all modules */
	void clear();

	/** @brief returns a list of all modules in this simulation */
	std::list<Module*> getModules(){return modules;};

	/** @brief returns a list of all links in this simulation */
	std::list<Link*> getLinks(){return links;};

	/** @brief Tests if the given link would be valid regarding in- and out-port. 
	With logoutbut one can surpress logger warnings. */
	bool isLinkingValid(Module* source, std::string outPort, 
						Module* dest, std::string inPort, bool logOutput);

	/** @brief adds an observer, providing state change information of the module */
	void addObserver(SimulationObserver *obs);

	/** @brief removes an observer */
	void removeObserver(SimulationObserver *obs);

	/** @brief import a simulation from a device-source 
	@param[in]	source				the source-device
	@param[in]	filepath			is used for resolving relative paths
	@param[out] modemap				provides a identification map for further file-processing
	@param[in]	overwrittenOwner	the anchor group, causes undefined behaviour if of type group
	NULL will overwrite the parent-group with to root-group
	@param[in]	overwriteGroupOwner	indicates if overwirttenOwner should be used */
	bool loadSimulation(QIODevice* source, QString filepath, 
		std::map<std::string, DM::Module*>& modMap, 
		DM::Module* overwrittenOwner = NULL, bool overwriteGroupOwner = false);

	/** @brief exports a simulation to a device in xml-like format */
	void writeSimulation(QIODevice* dest, QString filePath);

	/** @brief checks the whole simulation stream for possible missing views */
	bool checkStream();

	/** @brief set simulation config e.g. EPSG code...**/
	void setSimulationConfig(SimulationConfig config);

	/** @brief returns simulation config **/
	SimulationConfig getSimulationConfig();

protected:
	/** @brief returns the inport data of the link, taking group ports into account */
	ISystem* getData(Link* l);

	/** @brief shifts the data from one link side to another, taking groups into account */
	void	shiftData(Link* l, bool successor = false);

private:
	/** @brief shifts data from the outgoing port of a module to the inport of the successor module
	returns destination module */
	std::set<Module*> shiftModuleOutput(Module* m);

	/** @brief shifts data from the outgoing port of a group to the inport of the successor module
	returns destination module */
	std::set<Module*> shiftGroupInput(Group* m);

	/** @brief checks the stream beginning with this module for possible missing views */
	bool checkModuleStreamForward(Module* m);

	/** @brief checks the stream beginning with this link for possible missing views */
	bool checkModuleStreamForward(Link* link);

	/** @brief checks the stream beginning with this group for possible missing views */
	bool checkGroupStreamForward(Group* g, std::string streamName, bool into);

	/** @brief if true, print custom status updates */
	bool withStatusUpdates;

	/** @brief the file to print the status updates to */
	std::ofstream statusFile;

	/** @brief emit a status update */
	void emitStatusUpdate(float progress, const std::string& msg);

public:
	/** @brief returns all links connected to this port */
	std::vector<Link*> getIngoingLinks(const Module* dest, const std::string& inPort) const;

	/** @brief returns all links connected to this port */
	std::vector<Link*> getOutgoingLinks(const Module* src, const std::string& outPort) const;

	/** @brief returns all links connected to this port */
	std::vector<Link*> getIntoGroupLinks(const Module* src, const std::string& inPort) const;

	/** @brief returns all links connected to this port */
	std::vector<Link*> getOutOfGroupLinks(const Module* dest, const std::string& outPort) const;

	/** @brief serialise simuatlion */
	std::string serialise();

	/** @brief print status updates to given file path */
	void installStatusUpdater(const std::string& path);

private:
	bool canceled;
	std::list<Module*>	modules;
	std::list<Link*>	links;
	SimulationStatus	status;
	ModuleRegistry*		moduleRegistry;
	std::vector<SimulationObserver*>	observers;
	SimulationConfig	config;
};

}
#endif // SIMULATION_H
