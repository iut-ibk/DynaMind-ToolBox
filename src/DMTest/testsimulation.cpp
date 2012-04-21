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
#include <dynamicinout.h>
#include <grouptest.h>
#include <dmporttuple.h>

void TestSimulation::addModuleToSimulationTest(){
    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
    DM::Logger(DM::Standard) << "Add Module";
    DM::Simulation sim;
    sim.registerNativeModules("dynamind-testmodules");
    DM::Module * m = sim.addModule("TestModule");
    QVERIFY(m != 0);
}



void TestSimulation::loadModuleNativeTest() {
    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
    DM::Logger(DM::Standard) << "Load Native Module";
    DM::Simulation sim;
    QVERIFY(sim.registerNativeModules("dynamind-testmodules") == true);
}




void TestSimulation::repeatedRunTest() {
    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
    DM::Logger(DM::Standard) << "Test Repeatet Run";
    DM::Simulation sim;
    sim.registerNativeModules("dynamind-testmodules");
    DM::Module * m = sim.addModule("TestModule");
    QVERIFY(m != 0);
    for (long i = 0; i < 10; i++) {
        sim.run();
        QVERIFY(sim.getSimulationStatus() == DM::SIM_OK);
    }

}
void TestSimulation::linkedModulesTest() {
    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
    DM::Logger(DM::Standard) << "Test Linked Modules";
    DM::Simulation sim;
    sim.registerNativeModules("dynamind-testmodules");
    DM::Module * m = sim.addModule("TestModule");

    QVERIFY(m != 0);
    DM::Module * inout  = sim.addModule("InOut");

    QVERIFY(inout != 0);
    DM::ModuleLink * l = sim.addLink(m->getOutPort("Sewer"), inout->getInPort("Inport"));

    QVERIFY(l != 0);
    for (long i = 0; i < 10; i++){
        sim.run();
        QVERIFY(sim.getSimulationStatus() == DM::SIM_OK);
    }
}

void TestSimulation::linkedDynamicModules() {
    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
    DM::Logger(DM::Standard) << "Test Linked Modules";
    DM::Simulation sim;
    sim.registerNativeModules("dynamind-testmodules");
    DM::Module * m = sim.addModule("TestModule");
    QVERIFY(m != 0);
    DM::Module * inout  = sim.addModule("InOut");
    QVERIFY(inout != 0);
    DM::ModuleLink * l = sim.addLink(m->getOutPort("Sewer"), inout->getInPort("Inport"));
    QVERIFY(l != 0);
    DynamicInOut * dyinout  = (DynamicInOut *)sim.addModule("DynamicInOut");
    QVERIFY(dyinout != 0);
    dyinout->addAttribute("D");

    //Run Simulation update to create outport and allow linking
    //sim.startSimulation(true);
    DM::ModuleLink * l1 = sim.addLink(inout->getOutPort("Inport"), dyinout->getInPort("Inport"));
    QVERIFY(l1 != 0);
    DM::Module * inout2  = sim.addModule("InOut2");
    QVERIFY(inout2 != 0);
    DM::ModuleLink * l2 = sim.addLink(dyinout->getOutPort("Inport"), inout2->getInPort("Inport"));
    QVERIFY(l2 != 0);
    sim.run();
    QVERIFY(sim.getSimulationStatus() == DM::SIM_OK);
}

void TestSimulation::linkedDynamicModulesOverGroups() {
    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
    DM::Logger(DM::Standard) << "Test Linked Modules";
    DM::Simulation sim;
    sim.registerNativeModules("dynamind-testmodules");
    DM::Module * m = sim.addModule("TestModule");
    QVERIFY(m != 0);
    DM::Module * inout  = sim.addModule("InOut");
    QVERIFY(inout != 0);
    DM::ModuleLink * l = sim.addLink(m->getOutPort("Sewer"), inout->getInPort("Inport"));
    QVERIFY(l != 0);
    //Here comes the group
    GroupTest * g = (GroupTest * ) sim.addModule("GroupTest");
    g->addInPort("In");
    DM::ModuleLink * l_in = sim.addLink(inout->getOutPort("Inport"),g->getInPortTuple("In")->getInPort());
    QVERIFY(l_in != 0);
    g->addOutPort("Out");
    DynamicInOut * dyinout  = (DynamicInOut *)sim.addModule("DynamicInOut");
    QVERIFY(dyinout != 0);
    dyinout->setGroup(g);
    QVERIFY(dyinout != 0);
    dyinout->addAttribute("D");
    //Run Simulation update to create outport and allow linking
    //sim.startSimulation(true);
    DM::ModuleLink * l1 = sim.addLink(g->getInPortTuple("In")->getOutPort(), dyinout->getInPort("Inport"));
    QVERIFY(l1 != 0);
    DM::ModuleLink * l_out = sim.addLink(dyinout->getOutPort("Inport"), g->getOutPortTuple("Out")->getInPort());
    QVERIFY(l_out != 0);
    DM::Module * inout2  = sim.addModule("InOut2");
    QVERIFY(inout2 != 0);
    DM::ModuleLink * l2 = sim.addLink(g->getOutPortTuple("Out")->getOutPort(), inout2->getInPort("Inport"));
    QVERIFY(l2 != 0);
    sim.run();
    QVERIFY(sim.getSimulationStatus() == DM::SIM_OK);
}


QTEST_MAIN( TestSimulation )
#include "testsimulation.moc"

