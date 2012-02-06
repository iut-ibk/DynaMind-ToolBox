/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @author  Michael Mair <abroxos@gmail.com>
 * @version 1.0
 * @section LICENSE
 * This file is part of DynaMite
 *
 * Copyright (C) 2011  Christian Urich, Michael Mair

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

#include "compilersettings.h"

#include <iostream>
#include <simulation.h>
#include <dmdatabase.h>
#include <vibe_log.h>
#include <datamanagement.h>
#include <QThreadPool>
#include <module.h>
#include <DM.h>
#include <sstream>
#include <pythonenv.h>


using namespace std;
using namespace DM;



int main(int argc, char *argv[], char *envp[]) {
    //Init Logger
    ostream *out = &cout;
    DM::Log::init(new OStreamLogSink(*out), DM::Debug);
    DM::Logger(DM::Debug) << "Start";


    DM::PythonEnv *env = DM::PythonEnv::getInstance();

    DataManagement::init();
    DMDatabase * db = new DMDatabase();
    DataManagement::getInstance().registerDataBase(db);   //Init Logger
    Simulation * sim = new Simulation;

    sim->registerNativeModules("cityblock");
    sim->registerNativeModules("dynamindsewer");

    //Load Modules
    DM::Module * sb = sim->addModule("SuperBlock");
    DM::Module * cb = sim->addModule("CityBlock");
    DM::Module * ssl = sim->addModule("SimpleSewerLayout");
    DM::Module * tm = sim->addModule("TimeAreaMethod");
    DM::Module * swmm = sim->addModule("DMSWMM");

    //Set Parameter
    swmm->setParameterValue("RainFile", "/home/c8451045/Documents/vibe2/data/raindata/euler_II_133_120_5.dat");


    sim->addLink(sb->getOutPort("City"), cb->getInPort("City"));
    sim->addLink(cb->getOutPort("City"), ssl->getInPort("City"));
    sim->addLink(ssl->getOutPort("City"), tm->getInPort("City"));
    sim->addLink(tm->getOutPort("City"), swmm->getInPort("City"));
    sim->run();

    sim->writeSimulation("/tmp/test.vib");


    QThreadPool::globalInstance()->waitForDone();

    //sim->run(true)





    QThreadPool::globalInstance()->waitForDone();
    delete sim;
    /*DM::Logger(DM::Debug) << "End";

    Simulation * loadsim = new Simulation;
    loadsim->registerNativeModules("cityblock");
    loadsim->registerNativeModules("dynamindsewer");

    loadsim->loadSimulation("/tmp/test.vib");
    loadsim->run(false);



    QThreadPool::globalInstance()->waitForDone();*/
    return true;
}
