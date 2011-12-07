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
using namespace vibens;

bool DynaMiteTest();
bool DMBaseTest();

int main(int argc, char *argv[], char *envp[]) {

    //Init Logger
    ostream *out = &cout;
    vibens::Log::init(new OStreamLogSink(*out), vibens::Debug);
    vibens::Logger(vibens::Debug) << "Start";

    if(!DynaMiteTest())
       Logger(Error) << "DynaMiteTest FAILED";
   else
        Logger() << "DynaMiteTest DONE";

  /*  if(!DMBaseTest())
        Logger(Error) << "DMBaseTest FAILED";
    else
        Logger() << "DMBaseTest DONE";

    if(!DMBaseTest())
        Logger(Error) << "DMBaseTest_2 FAILED";
    else
        Logger() << "DMBaseTest_2 DONE";*/

    return 1;
}

bool DynaMiteTest()
{
    vibens::PythonEnv *env = vibens::PythonEnv::getInstance();
    env->addPythonPath("/home/csae6550/work/VIBe2Core/build/Release/");

    QThreadPool::globalInstance()->setMaxThreadCount(1);
    DataManagement::init();
    DMDatabase * db = new DMDatabase();
    DataManagement::getInstance().registerDataBase(db);   //Init Logger
    Simulation * sim = new Simulation;
    sim->registerNativeModules("dmtestmodule");
    sim->registerPythonModules("/home/csae6550/work/VIBe2Core/scripts");
    vibens::Module * in = sim->addModule("TestModule");
    vibens::Module * outm =sim->addModule("InOut");
    vibens::Module * outm2 =sim->addModule("InOut");
    vibens::Module * outm3 = sim->addModule("WhiteNoise");
    vibens::Module * outm4 = sim->addModule("ImportShapeFile");
    sim->addLink(outm4->getOutPort("Network"),outm->getInPort("Inport"));
    //sim->addLink(in->getOutPort("Sewer"), outm->getInPort("Inport"));
    sim->addLink(outm->getOutPort("Inport"), outm2->getInPort("Inport"));
    sim->addLink(in->getOutPort("Sewer"),outm3->getInPort("Inport"));
    sim->run();

    QThreadPool::globalInstance()->waitForDone();
    delete sim;
    vibens::Logger(vibens::Debug) << "End";

    return true;
}
