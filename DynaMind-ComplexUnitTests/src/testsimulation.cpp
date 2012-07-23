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
#include <testsimulation.h>
#include <dmmodule.h>
#include <dmsimulation.h>
#include <dmlog.h>
#include <dmporttuple.h>
#include <dmpythonenv.h>
#include <QDir>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

namespace {
void TestSimulation::SetUp()
{
    srand ( time(NULL) );
    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);
    DM::Logger(DM::Debug) << "Load Native Module";
    DM::PythonEnv::getInstance()->addPythonPath(QDir::currentPath().toStdString());
    sim = new DM::Simulation();


    ASSERT_TRUE(sim->registerNativeModules("Modules/dynamind-basicmodules") == true);
    ASSERT_TRUE(sim->registerNativeModules("Modules/cityblock") == true);
    ASSERT_TRUE(sim->registerNativeModules("Modules/dynamind-sewer") == true);

    sim->registerPythonModules("PythonModules/scripts/");
    sim->loadSimulation("Data/Simulations/modell_chse.dyn");
    for(int i = 0; i < 1; i++)
        sim->run();
    ASSERT_TRUE(sim->getSimulationStatus() == DM::SIM_OK);
}

TEST_F(TestSimulation,TestModuleUpdate) {

    DM::Module * gn = sim->getModuleByName("Generation");
    ASSERT_TRUE(gn->isExecuted());

    //Test Rerun is fine
    sim->startSimulation(true);

    DM::Module * cb = sim->getModuleByName("cityblock");
    ASSERT_TRUE(cb->isExecuted());

    gn = sim->getModuleByName("Generation");
    ASSERT_TRUE(gn->isExecuted());


    DM::Module * mexcon = sim->getModuleByName("ExConduits");
    ASSERT_TRUE(mexcon->isExecuted());

    //Change stuff
    mexcon->setExecuted(false);
    sim->startSimulation(true);
    DM::Module * mexcat = sim->getModuleByName("ExCatchments");

    //If ExConduits has changed ExCatchments should be uneffected
    ASSERT_TRUE(mexcat->isExecuted());



}

TEST_F(TestSimulation,RepeatedUpdatesAndRandomChanges) {


    DM::Module * cb = sim->getModuleByName("cityblock");
    cb->setExecuted(false);
    sim->startSimulation(true);
    DM::Module * gn = sim->getModuleByName("GenNetwork");
    ASSERT_FALSE(gn->isExecuted());
    DM::Group * gen = (DM::Group *) sim->getModuleByName("Generation");
    ASSERT_TRUE(gen->HasContaingModuleChanged());
    sim->startSimulation(false);


    std::vector<DM::Module*> vm = sim->getModules();
    for (int i = 0; i < 200; i++)  {
        for (int j = 0; j < 10; j++)      {
            int r = rand() % vm.size();
            DM::Module * m =  vm[r];
            m->setExecuted(false);
            DM::Logger(DM::Standard) << "Changed Module " <<  m->getUuid() << m->getClassName();
            sim->startSimulation(true);

        }
        sim->startSimulation(false);

    }
}

void TestSimulation::TearDown()
{
    delete sim;
}

}
