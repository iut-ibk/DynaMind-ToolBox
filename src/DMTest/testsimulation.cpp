/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2012  Christian Urich
 
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

#include <ostream>
#include <QtTest/QtTest>
#include "testsimulation.h"
#include <dmmodule.h>
#include <dmsimulation.h>
#include <dmlog.h>

/** @brief Test adding Module */
void TestSimulation::addModuleToSimulationTest(){
    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
    DM::Logger(DM::Standard) << "Add Module";
    DM::Simulation sim;
    sim.registerNativeModules("dynamind-testmodules");
    DM::Module * m = sim.addModule("TestModule");
    QVERIFY(m != 0);
}


/** @brief Test if the a native module is loaded correctly */
void TestSimulation::loadModuleNativeTest() {
    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
    DM::Logger(DM::Standard) << "Load Native Module";
    DM::Simulation sim;
    QVERIFY(sim.registerNativeModules("dynamind-testmodules") == true);
}



/** @brief Test repeated Simulation execution Module */
void TestSimulation::repeatedRunTest() {
    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
    DM::Logger(DM::Standard) << "Test Repeatet Run";
    DM::Simulation sim;
    sim.registerNativeModules("dynamind-testmodules");
    DM::Module * m = sim.addModule("TestModule");
    QVERIFY(m != 0);
    for (long i = 0; i < 200; i++)
        sim.run();
    QVERIFY(true==true);
}

/** @brief Test Linked Modules Module */
void TestSimulation::linkedModulesTest() {
    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
    DM::Logger(DM::Standard) << "Test Linked Modules";
    DM::Simulation sim;
    sim.registerNativeModules("dynamind-testmodules");
    DM::Module * m = sim.addModule("TestModule");
    if (!m)
        QVERIFY(m != 0);
    DM::Module * inout  = sim.addModule("InOut");
    if (!inout)
        QVERIFY(inout != 0);
    DM::ModuleLink * l = sim.addLink(m->getOutPort("Sewer"), inout->getInPort("Inport"));
    if (!l)
        QVERIFY(l != 0);
    for (long i = 0; i < 200; i++)
        sim.run();
    QVERIFY(true==true);
}


QTEST_MAIN( TestSimulation )
#include "testsimulation.moc"
