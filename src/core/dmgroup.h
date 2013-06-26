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

#ifndef DMGROUP_H
#define DMGROUP_H
#include <dmmodule.h>

namespace DM {

class DM_HELPER_DLL_EXPORT Group: public Module
{
public:
	/*Group()
	{
		addParameter("IN-ports", DM::STRING_LIST, &inports);
		addParameter("OUT-ports", DM::STRING_LIST, &inports);
	}*/

	void init()
	{
	}
private:
protected:
	std::vector<std::string> nameOfInPorts;
	std::vector<std::string> nameOfOutPorts;
	//std::vector<Module*> modules;
	//std::vector<std::string> inports;
};

}

#ifdef SIMENV_GROUP

#include <vector>


class QMutex;
class QRunnable;



namespace DM {

    class PortTuple;

    /**
     * @ingroup DynaMind-Core
     * @brief Groups are used to enable grouping and repeated execution
     * of modules.
     *
     * It is derived from DM::Module. To enable the grouping of modules
     * a module container has been added. Since DM::Group is derived from
     * DM::Module also a DM::Group can be added.
     * To repeatedly executed a Group the member variable Steps can be set.
     * The member variable step is used as counter. A group is executed as
     * long as step > Steps. @TODO: Check is the interalCounter of the module
     * can be used as step variable.
     *
     * The Group is also used to (or misused) to execute the containing modules.
     * To enable multi threading the modules - as well as the derived groups -
     * The module is put in a  DM::ModuleRunnable object. DM::Modulerunnable not
     * only calls the run method of the module it also handels the pre and post
     * processing needed for the module execution.
     *
     * To connect modules within the group with data from outside tuple ports are used.
     * Tuple ports can be used to get data in a group (DM::INPORTTUPLE) or export data from the
     * group (DM::OUTPORTTUPLE). A port tuple consits of an inport and an outport.
     *
     */
    class DM_HELPER_DLL_EXPORT  Group : public Module
    {
    private:
        //::QMutex * mutex;
        std::vector<Module*> modules;
        std::vector<PortTuple * > inPortTuple;
        std::vector<PortTuple * > outPortTuple;


         std::vector<Module * > notUsedModules;
         std::vector<Module * > UsedModules;
         std::vector<Module * > currentRunning;

         QVector<QRunnable *> getNextJobs();
         bool moduleHasChanged;

		 Port* getPort(std::string name) const;
    protected:
         int Steps;
         int curStep;
    public:
         /** @brief default constructor for the group
           *
           * creates recursive mutex and sets step to 0
           */
        Group();
        /** @brief Adds module to the group, ownership is taken by the group */
        void addModule(Module * module);
        /** @brief Removes module from a group, just removes the group from the list, module still exists */
        void removeModule(Module * module);
        /** @brief Returns list of modules within the group*/
        std::vector<Module*> getModules(){return this->modules;}
        /** @brief Adds new tuple port */
        PortTuple * addTuplePort(std::string LinkedDataName, int PortType);
        /** @brief Remove tuple port */
        void removeTuplePort(PortTuple * pt);
        /** @brief Returns pointer to in port tuple by name */
        PortTuple * getInPortTuple(std::string name) const;
        /** @brief Returns pointer to out port tuple by name */
        PortTuple * getOutPortTuple(std::string name) const;
        /** @brief Returns in port tuples as pointer vector */
        std::vector<PortTuple * >  getInPortTuples(){return this->inPortTuple;}
        /** @brief Returns out port tuples as pointer vector */
        std::vector<PortTuple * >  getOutPortTuples(){return this->outPortTuple;}
        /** @brief A Destructor :-), I'm sure their was a very good reason to not put this make an extra destructure... just can't rember why */
        virtual void Destructor();
        /** @brief Returns true */
        virtual bool isGroup() const{return true;}
        /** @brief Returns in port from port tuple,  if outport tuple and inport tuple have the same name, the in port from the outport tuple is returned
         * @TODO remove this method in the next version
         */
        virtual Port * getInPort(std::string Name) const;
        /** @brief Returns out port from port tuple,  if outport tuple and inport tuple have the same name, the in port from the outport tuple is returned
         * @TODO remove this method in the next version
         */
        virtual Port * getOutPort(std::string Name) const;
        /** @brief Used after a module has been executed
         *
         * After a module the run method of a module is executed it is removed from the notUsedModules
         * list and the currentRunning list. If m is a group, the steps are reset.
         * finishedModules also starts the next modules!
         *  @TODO see run method, seperate the group from the simulation execution
         */
        void finishedModule(Module * m);
        /** @brief starts execution of modules related to the group, increase by 1*/
        virtual void run();
        /** @brief Returns true if still runnable.
         *
         * Before a group is executed step is set to 0. After every run step is raised by one.
         * A group is executed as long as step < Steps. Steps can be set from a derived class. Is used
         * for for loops
         */
        //bool isRunnable(){return step < Steps;}
		int StepsLeft() {return Steps-curStep;};
        /** @brief Resets all containing modules and step to 0 */
        void resetSteps();
        /** @brief Clears Module list, modules still exist!*/
        void clearModules();
        /** @brief Resets all Modules */
        void resetModules();
        /** @brief Calls the Destructor, Module destructor and deletes the mutex*/
        virtual ~Group();

        /** @brief Set Steps */
        void setStep(int s) {this->curStep = s;}

        /** @brief Return Steps */
        int getSteps() {return this->Steps;}

        /** @brief Set true if module within the group has changed */
        void setContentOfModuleHasChanged(bool c);

        /** @brief Returns true if a module within the group has changed */
        bool HasContaingModuleChanged() {return this->moduleHasChanged;}

        virtual void setExecuted(bool ex);

    };
}

#define DM_DECLARE_GROUP(node)  \
        public: \
            static const char *classname; \
            static const char *filename; \
            const char *getClassName(); \
            const char *getFileName(); \
        private:

#define  DM_DECLARE_GROUP_NAME(nodename, module) \
            const char *nodename::classname = #nodename; \
                                              const char *nodename::getClassName()  { return nodename::classname; } \
                                              const char *nodename::filename = #module; \
                                                                               const char *nodename::getFileName()  { return nodename::filename; }


#endif // SIMENV_GROUP

#endif // GROUP_H
