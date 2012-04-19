/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @author  Michael Mair <abroxos@gmail.com>
 * @version 1.0
 * @section LICENSE
 * This file is part of DynaMite
 *
 * Copyright (C) 2011-2012  Christian Urich, Michael Mair

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

#include "dmcompilersettings.h"
#include "dmhelper.h"

#include <iostream>
#include <dmsimulation.h>
#include <dmlog.h>
#include <dmdatamanagement.h>
#include <QThreadPool>
#include <dmmodule.h>
#include <dm.h>
#include <sstream>
#include <dmpythonenv.h>
#include <complexgeometry.h>
#include <QStringList>

using namespace std;
using namespace DM;

bool TestLoadModuleNative();
bool TestAddModule();
bool TestRepeatedRun();
bool TestLinkedModules();

int main(int argc, char *argv[], char *envp[])
{
    ostream *out = &cout;
    DM::Log::init(new OStreamLogSink(*out), DM::Standard);
    DM::Logger(DM::Debug) << "Nothing to test";
    //DM::PythonEnv *env = DM::PythonEnv::getInstance();
    //env->addOverWriteStdCout();

    if (TestLoadModuleNative() ) {
        DM::Logger(DM::Standard) << "OK";
    } else {
        DM::Logger(DM::Standard) << "FAILED";
    }

    if (TestAddModule() ) {
        DM::Logger(DM::Standard) << "OK";
    } else {
        DM::Logger(DM::Standard) << "FAILED";
    }
    if (TestAddModule() ) {
        DM::Logger(DM::Standard) << "OK";
    } else {
        DM::Logger(DM::Standard) << "FAILED";
    }
    if (TestRepeatedRun() ) {
        DM::Logger(DM::Standard) << "OK";
    } else {
        DM::Logger(DM::Standard) << "FAILED";
    }
    if (TestLinkedModules() ) {
        DM::Logger(DM::Standard) << "OK";
    } else {
        DM::Logger(DM::Standard) << "FAILED";
    }


    return 1;
}



/** @brief Test if the a native module is loaded correctly */
bool TestLoadModuleNative() {
    DM::Logger(DM::Standard) << "Load Native Module";
    DM::Simulation sim;
    return sim.registerNativeModules("dynamind-testmodules");
}


/** @brief Test adding Module */
bool TestAddModule() {
    DM::Logger(DM::Standard) << "Add Module";
    DM::Simulation sim;
    sim.registerNativeModules("dynamind-testmodules");
    DM::Module * m = sim.addModule("TestModule");
    if (!m)
        return false;
    return true;
}

/** @brief Test repeated Simulation execution Module */
bool TestRepeatedRun() {
    DM::Logger(DM::Standard) << "Test Repeatet Run";
    DM::Simulation sim;
    sim.registerNativeModules("dynamind-testmodules");
    DM::Module * m = sim.addModule("TestModule");
    if (!m)
        return false;
    for (long i = 0; i < 200; i++)
        sim.run();
    return true;
}

/** @brief Test Linked Modules Module */
bool TestLinkedModules() {
    DM::Logger(DM::Standard) << "Test Linked Modules";
    DM::Simulation sim;
    sim.registerNativeModules("dynamind-testmodules");
    DM::Module * m = sim.addModule("TestModule");
    if (!m)
        return false;
    DM::Module * inout  = sim.addModule("InOut");
    if (!inout)
        return false;
    DM::ModuleLink * l = sim.addLink(m->getOutPort("Sewer"), inout->getInPort("Inport"));
    if (!l)
        return false;
    for (long i = 0; i < 200; i++)
        sim.run();
    return true;
}
