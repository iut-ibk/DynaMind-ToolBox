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
#ifndef GROUP_H
#define GROUP_H
#include "compilersettings.h"
#include <module.h>
#include <vector>


class QMutex;
class QRunnable;



namespace DM {

    class PortTuple;


    class DM_HELPER_DLL_EXPORT  Group : public Module
    {
    private:
        ::QMutex * mutex;
        std::vector<Module*> modules;
        std::vector<PortTuple * > inPortTuple;
        std::vector<PortTuple * > outPortTuple;


         std::vector<Module * > notUsedModules;
         std::vector<Module * > UsedModules;
         std::vector<Module * > currentRunning;

         QVector<QRunnable *> getNextJobs();
    protected:
         int Steps;
         int step;
    public:
        Group();
        void addModule(Module * module);
        void removeModule(Module * module);
        std::vector<Module*> getModules(){return this->modules;}
        PortTuple * addTuplePort(std::string LinkedDataName, int PortType);
        PortTuple * getInPortTuple(std::string name);
        std::vector<PortTuple * >  getInPortTuples(){return this->inPortTuple;}
        std::vector<PortTuple * >  getOutPortTuples(){return this->outPortTuple;}
        PortTuple * getOutPortTuple(std::string name);
        virtual void Destructor();
        virtual bool isGroup() const{return true;}

        virtual std::string getParameterAsString(std::string Name);
        virtual Port * getInPort(std::string Name);
        virtual Port * getOutPort(std::string Name);
        void finishedModule(Module * m);
        virtual void run();
        bool isRunnable(){return step < Steps;}
        void resetSteps(){this->step = 0;}
        void clearModules();
        void resetModules();
        virtual ~Group();

    };
}

#define VIBe_DECLARE_GROUP(node)  \
        public: \
            static const char *classname; \
            static const char *filename; \
            const char *getClassName(); \
            const char *getFileName(); \
        private:

#define  VIBe_DECLARE_GROUP_NAME(nodename, module) \
            const char *nodename::classname = #nodename; \
                                              const char *nodename::getClassName()  { return nodename::classname; } \
                                              const char *nodename::filename = #module; \
                                                                               const char *nodename::getFileName()  { return nodename::filename; }
#endif // GROUP_H
