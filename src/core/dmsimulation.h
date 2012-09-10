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

#ifndef DMSIMULATION_H
#define DMSIMULATION_H

#include "dmcompilersettings.h"
#include <string>
#include <vector>
#include <QThread>
#include <dmrootgroup.h>


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

    enum SimuatlonsStatuses {
        SIM_OK,
        SIM_FAILED_LOAD,
        SIM_ERROR_SYSTEM_NOT_SET

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
    class DM_HELPER_DLL_EXPORT Simulation:public QThread {
    public:

        /** @brief creates a new simulation
         *
         * Every simulation comes with a root group to start with.
        */
        Simulation();
        virtual ~Simulation();

        /** @brief Starts the Simulation. Returns if model run was executed successful
         *
         * If the paramter virtualRun is true only the init methods of the modules are executed.
         * This is ca be used to update the data model.
         */
        bool startSimulation(bool virtualRun = false);

         /** @brief Starts the Simulation. Returns if model run was executed successful
          *
          * calles startSimulation with default parameter
          */
        virtual void run();

        /** @brief add data observer to the simulation */
        void addDataObserver(DataObserver*  observer);

        /** @brief add simulation observer to the simulation */
        void addSimulationObserver(SimulationObserver * simulationObserver);

        /** @brief return root group */
        Module * getRootGroup(){return rootGroup;}

        /** @brief adds a new link to the simulation. If it was not possible to create the link a null pointer is returned,
         * otherwise the a pointer to the new created link is returned.
         */
        ModuleLink * addLink(Port * OutPort,Port * InPort);

        /** @brief removes a link */
        void removeLink(ModuleLink * );

        /** @brief adds a new module/group to the system. If it was not possible to create the module a null pointer is returned,
         * otherwise a pointer to the new created module or group
         */
        Module * addModule(std::string ModuleName);

        /** @brief Removes module.
         *
         * Delets Module. Since the destructor is called the module is also removed from the Modulelist
         */
        void removeModule(std::string UUid);

        /** @brief returns a pointer to the ModuleRegistry */
        ModuleRegistry * getModuleRegistry();

        /** @brief returns all groups within the simulation */
        std::vector<Group*> getGroups();

        /** @brief writes the simulation to a xml file */
        void writeSimulation(std::string filename);

        /** @brief returns a vector to all modules */
        std::vector<Module*> getModules() const;

        /** @brief adds a simulation saved in a file to the current simulation */
        std::map<std::string, std::string>  loadSimulation(std::string FileName);

        /** @brief returns a pointer to a module */
        Module * getModuleWithUUID(std::string UUID);

        /** @brief returns the module according to the module name, if there are several modules with the same name the first one is returend */
        Module * getModuleByName(std::string name);

        /** @brief returns a vector to all links in the simulation */
        std::vector<ModuleLink*> getLinks();

        /** @brief Resets the module. Be careful to do so a new module is created which means the
         * pointer has changed as well as the uuid
         *
         * The method creates a new module/group, copies the parameters from the old to
         * the new module/group and rewires the module/group
         */
        Module * resetModule(std::string UUID);

        /** @brief Reset all modules */
        void resetModules();

        /** @brief Reloads all python modules */
        void reloadModules();

        /** @brief return all modules of the same type */
        std::vector<Module*> getModulesFromType(std::string name);

        /** @brief remove module */
        void deregisterModule(std::string UUID);

        /** @brief register a new native module returns if module has been loaded succcessfully*/
        bool registerNativeModules(std::string Filename);

        /** @brief Register pyhton modules.
         *
         * The function loads pyhton modules that are saved in a special data structure
         *
         * - ModuleRoot
         *      - MyModules.py
         *      - MyModules
         *          - __init__.py
         *          - mymodule.py
         */
        void registerPythonModules(std::string path);

        /** @brief Returns true if the simulation run is executed as virtual run */
        bool isVirtualRun(){return this->virtualRun;}

        /** @brief Add the modules set in the QSetting **/
        bool addModulesFromSettings();

        void loadPythonModulesFromDirectory(std::string path);
        /** @brief Add Modules from default location
          * modules/
        **/
        void loadModulesFromDefaultLocation();
        /** @brief after a Simulation is executed this parameter returns if something happend in between the simulation */
        int getSimulationStatus();

        /** @brief set Simulation Status */
        void setSimulationStatus(int Status);

    private:
        DMRootGroup * rootGroup;
        std::map<std::string, Module*> Modules;
        //std::vector<ModuleLink * > Links;
        SimulationPrivate *data;
        IDataBase * database;
        ModuleRegistry * moduleRegistry;
        void removeLinksFromModule(Module *);




        bool virtualRun;



    };
}
#endif // SIMULATION_H
