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
#include <performnacetests.h>
#include <dmmodule.h>
#include <dmsimulation.h>
#include <dmlog.h>
//#include <dmporttuple.h>
#include <dmpythonenv.h>
#include <QDir>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

namespace {
void PerformanceTest::SetUp()
{
    /*ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Debug);
    DM::Logger(DM::Debug) << "Load Native Module";
    DM::PythonEnv::getInstance()->addPythonPath(QDir::currentPath().toStdString());
    sim = new DM::Simulation();


    ASSERT_TRUE(sim->registerNativeModules("Modules/dynamind-basicmodules") == true);
    ASSERT_TRUE(sim->registerNativeModules("Modules/cityblock") == true);
    ASSERT_TRUE(sim->registerNativeModules("Modules/dynamind-sewer") == true);
    ASSERT_TRUE(sim->registerNativeModules("Modules/powervibe") == true);
    //sim->registerPythonModules("PythonModules/scripts/");
    sim->loadSimulation("Data/Simulations/performance_sewer_small.dyn");*/

}

TEST_F(PerformanceTest,PerformanceTestRun) {


    /*for(int i = 0; i < 1; i++)
        sim->run();
    ASSERT_TRUE(sim->getSimulationStatus() == DM::SIM_OK);*/


}


void PerformanceTest::TearDown()
{
    //delete sim;
}

}
