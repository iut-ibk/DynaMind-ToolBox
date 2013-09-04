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
#include <dmlogsink.h>
#include <dynamicinout.h>
#include <grouptest.h>

//#define OMPTEST
//#define GROUPTEST

#ifdef _OPENMP
#include <omp.h>
#endif

#define STDUNITTESTS

namespace {

/*
TEST_F(TestSimulation,validationtool) {
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
	DM::Logger(DM::Standard) << "Test validation tool";

	DM::Simulation sim;
	sim.registerModulesFromDirectory(QDir("./"));;
	DM::Module * mcreator = sim.addModule("CreateAllComponenets");
	//std::string uuid = mcreator->getUuid();
	ASSERT_TRUE(mcreator != 0);
	DM::Module * mcheck  = sim.addModule("CheckAllComponenets");
	ASSERT_TRUE(mcheck != 0);
	DM::ModuleLink * l2 = sim.addLink(mcreator->getOutPort("sys"), mcheck->getInPort("sys"));
	ASSERT_TRUE(l2 != 0);
	sim.run();
	ASSERT_TRUE(sim.getSimulationStatus() == DM::SIM_OK);
}*/

/* DOES NOT WORK ANYMORE (VALIDATION TOOL ON SUCCESSORSTATE)
TEST_F(TestSimulation,sqlsuccessortest) {
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
	DM::Logger(DM::Standard) << "Test Successor states (SQL)";
	DM::Simulation sim;
	sim.registerModulesFromDirectory(QDir("./"));;
	DM::Module * mcreator = sim.addModule("CreateAllComponenets");
	ASSERT_TRUE(mcreator != 0);
	DM::Module * msucc  = sim.addModule("SuccessorCheck");
	ASSERT_TRUE(msucc != 0);
	DM::ModuleLink * l1 = sim.addLink(mcreator->getOutPort("sys"), msucc->getInPort("sys"));
	ASSERT_TRUE(l1 != 0);
	sim.run();
	ASSERT_TRUE(sim.getSimulationStatus() == DM::SIM_OK);
}*/

#ifdef STDUNITTESTS

TEST_F(TestSimulation,testMemory){
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
	DM::Logger(DM::Standard) << "Add Module";
	DM::Simulation sim;
	sim.registerModulesFromDirectory(QDir("./"));
	DM::Module * m = sim.addModule("CreateNodes");
	//std::string m_uuid = m->getUuid();
	ASSERT_TRUE(m != 0);
	sim.run();
	ASSERT_TRUE(sim.getSimulationStatus() == DM::SIM_OK);
	for (int i = 0; i < 5; i++) {
		sim.removeModule(m);
		DM::Logger(DM::Standard) << "#" << i;
		m = sim.addModule("CreateNodes");
		ASSERT_TRUE(m != 0);
		sim.run();
	}
}

TEST_F(TestSimulation,addModuleToSimulationTest){
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
	DM::Logger(DM::Standard) << "Add Module";
	DM::Simulation sim;
	sim.registerModulesFromDirectory(QDir("./"));;
	DM::Module * m = sim.addModule("TestModule");
	ASSERT_TRUE(m != 0);
}

TEST_F(TestSimulation,loadModuleNativeTest) {
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
	DM::Logger(DM::Standard) << "Load Native Module";
	DM::Simulation sim;
#ifdef _WIN32
	ASSERT_TRUE(sim.registerModule("dynamind-testmodules.dll") == true);
#else
	ASSERT_TRUE(sim.registerModule("./libdynamind-testmodules") == true);
#endif
}

TEST_F(TestSimulation,repeatedRunTest) {
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
	DM::Logger(DM::Standard) << "Test Repeatet Run";
	DM::Simulation sim;
	sim.registerModulesFromDirectory(QDir("./"));;
	DM::Module * m = sim.addModule("TestModule");
	ASSERT_TRUE(m != 0);
	for (long i = 0; i < 100; i++) {
		sim.run();
		ASSERT_TRUE(sim.getSimulationStatus() == DM::SIM_OK);
	}
	ASSERT_TRUE(m->getStatus() == MOD_EXECUTION_OK);
}

#if defined _OPENMP && defined OMPTEST
TEST_F(TestSimulation,openmptest) {
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
	DM::Logger(DM::Standard) << "Test openmp redirecting";
	DM::Simulation sim;
	sim.registerModulesFromDirectory(QDir("./"));;
	DM::Module * m = sim.addModule("TestModule");
	ASSERT_TRUE(m != 0);
	int testThreadNumber = 3;
	omp_set_num_threads(testThreadNumber);
	sim.run();
	ASSERT_TRUE(sim.getNumOMPThreads() == testThreadNumber);
	ASSERT_TRUE(sim.getSimulationStatus() == DM::SIM_OK);
}
#endif

TEST_F(TestSimulation,linkedModulesTest) {
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
	DM::Logger(DM::Standard) << "Test Linked Modules";
	DM::Simulation sim;
	sim.registerModulesFromDirectory(QDir("./"));;
	DM::Module * m = sim.addModule("TestModule");
	ASSERT_TRUE(m != 0);

	DM::Module * inout  = sim.addModule("InOut");
	ASSERT_TRUE(inout != 0);

	//DM::ModuleLink * l = sim.addLink(m->getOutPort("Sewer"), inout->getInPort("Inport"));
	//ASSERT_TRUE(l != 0);

	ASSERT_TRUE(sim.addLink(m, "Sewer", inout, "Inport"));

	for (long i = 0; i < 10; i++){
		sim.run();
		ASSERT_TRUE(sim.getSimulationStatus() == DM::SIM_OK);
	}
	ASSERT_TRUE(m->getStatus() == MOD_EXECUTION_OK);
	ASSERT_TRUE(inout->getStatus() == MOD_EXECUTION_OK);
}

TEST_F(TestSimulation,linkedDynamicModules) {
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
	DM::Logger(DM::Standard) << "Test Linked Modules";
	DM::Simulation sim;
	sim.registerModulesFromDirectory(QDir("./"));;
	DM::Module * m = sim.addModule("TestModule");
	ASSERT_TRUE(m != 0);
	DM::Module * inout  = sim.addModule("InOut");
	ASSERT_TRUE(inout != 0);
	//DM::ModuleLink * l = sim.addLink(m->getOutPort("Sewer"), inout->getInPort("Inport"));
	//ASSERT_TRUE(l != 0);
	ASSERT_TRUE(sim.addLink(m, "Sewer", inout, "Inport"));
	DynamicInOut * dyinout  = (DynamicInOut *)sim.addModule("DynamicInOut");
	ASSERT_TRUE(dyinout != 0);
	dyinout->addAttribute("D");
	//DM::ModuleLink * l1 = sim.addLink(inout->getOutPort("Inport"), dyinout->getInPort("Inport"));
	//ASSERT_TRUE(l1 != 0);
	ASSERT_TRUE(sim.addLink(inout, "Inport", dyinout, "Inport"));
	DM::Module * inout2  = sim.addModule("InOut2");
	ASSERT_TRUE(inout2 != 0);
	//DM::ModuleLink * l2 = sim.addLink(dyinout->getOutPort("Inport"), inout2->getInPort("Inport"));
	//ASSERT_TRUE(l2 != 0);
	ASSERT_TRUE(sim.addLink(dyinout, "Inport", inout2, "Inport"));
	sim.run();
	ASSERT_TRUE(sim.getSimulationStatus() == DM::SIM_OK);

	ASSERT_TRUE(m->getStatus() == MOD_EXECUTION_OK);
	ASSERT_TRUE(inout->getStatus() == MOD_EXECUTION_OK);
	ASSERT_TRUE(dyinout->getStatus() == MOD_EXECUTION_OK);
	ASSERT_TRUE(inout2->getStatus() == MOD_EXECUTION_OK);
}

TEST_F(TestSimulation,LoopGroupTest)
{
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
	DM::Logger(DM::Standard) << "Test group";
	DM::Simulation sim;
	sim.registerModulesFromDirectory(QDir("./"));;

	DM::Module* m = sim.addModule("TestModule");
	ASSERT_TRUE(m != 0);

	GroupTest* g = (GroupTest* ) sim.addModule("GroupTest");
	ASSERT_TRUE(g != 0);
	g->addWriteStream("port");

	DM::Module* inoutInGroup  = sim.addModule("InOut", g);
	ASSERT_TRUE(inoutInGroup != 0);

	DM::Module* inoutAfterGroup  = sim.addModule("InOut");
	ASSERT_TRUE(inoutAfterGroup != 0);

	ASSERT_TRUE(sim.addLink(m, "Sewer", g, "port"));
	ASSERT_TRUE(sim.addLink(g, "port", inoutInGroup, "Inport"));
	ASSERT_TRUE(sim.addLink(inoutInGroup, "Inport", g, "port"));
	ASSERT_TRUE(sim.addLink(g, "port", inoutAfterGroup, "Inport"));


	//DM::ModuleLink * l = sim.addLink(m->getOutPort("Sewer"), inout->getInPort("Inport"));
	//ASSERT_TRUE(l != 0);
	//Here comes the group
	/*
	ASSERT_TRUE(sim.addLink(inout, "Inport", g, "In"));
	DynamicInOut * dyinout  = (DynamicInOut *)sim.addModule("DynamicInOut");
	ASSERT_TRUE(dyinout != 0);
	dyinout->setGroup(g);
	ASSERT_TRUE(dyinout != 0);
	dyinout->addAttribute("D");
	DM::ModuleLink * l1 = sim.addLink(g->getInPortTuple("In")->getOutPort(), dyinout->getInPort("Inport"));
	ASSERT_TRUE(l1 != 0);
	DM::ModuleLink * l_out = sim.addLink(dyinout->getOutPort("Inport"), g->getOutPortTuple("Out")->getInPort());
	ASSERT_TRUE(l_out != 0);
	DM::Module * inout2  = sim.addModule("InOut2");
	ASSERT_TRUE(inout2 != 0);
	DM::ModuleLink * l2 = sim.addLink(g->getOutPortTuple("Out")->getOutPort(), inout2->getInPort("Inport"));
	ASSERT_TRUE(l2 != 0);
	*/
	sim.run();
	ASSERT_TRUE(sim.getSimulationStatus() == DM::SIM_OK);

	ASSERT_TRUE(m->getStatus() == MOD_EXECUTION_OK);
	ASSERT_TRUE(inoutInGroup->getStatus() == MOD_EXECUTION_OK);
	ASSERT_TRUE(inoutAfterGroup->getStatus() == MOD_EXECUTION_OK);

	sim.removeModule(inoutInGroup);

	/*ASSERT_TRUE(sim.addLink(g, "In", g, "Out"));
	sim.run();
	ASSERT_TRUE(sim.getSimulationStatus() == DM::SIM_OK);*/

}
#define GROUPTEST
#ifdef GROUPTEST

TEST_F(TestSimulation,linkedDynamicModulesOverGroups)
{
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
	DM::Logger(DM::Standard) << "Test Linked Modules";
	DM::Simulation sim;
	sim.registerModulesFromDirectory(QDir("./"));;
	DM::Module * m = sim.addModule("TestModule");
	ASSERT_TRUE(m != 0);
	DM::Module * inout  = sim.addModule("InOut");
	ASSERT_TRUE(inout != 0);
	//DM::ModuleLink * l = sim.addLink(m->getOutPort("Sewer"), inout->getInPort("Inport"));
	ASSERT_TRUE(sim.addLink(m, "Sewer", inout, "Inport"));
	//Here comes the group
	GroupTest * g = (GroupTest * ) sim.addModule("GroupTest");
	g->addWriteStream("port");
	//g->addInPort("In");
	ASSERT_TRUE(sim.addLink(inout,"Inport",g,"port"));
	//g->addOutPort("Out");
	DynamicInOut * dyinout  = (DynamicInOut *)sim.addModule("DynamicInOut", g);
	ASSERT_TRUE(dyinout != 0);
	//dyinout->setGroup(g);
	ASSERT_TRUE(dyinout != 0);
	dyinout->addAttribute("D");
	ASSERT_TRUE(sim.addLink(g,"port", dyinout,"Inport"));
	ASSERT_TRUE(sim.addLink(dyinout,"Inport", g,"port"));
	DM::Module * inout2  = sim.addModule("InOut2");
	ASSERT_TRUE(inout2 != 0);
	ASSERT_TRUE(sim.addLink(g,"port", inout2,"Inport"));
	sim.run();
	ASSERT_TRUE(sim.getSimulationStatus() == DM::SIM_OK);

	ASSERT_TRUE(m->getStatus() == MOD_EXECUTION_OK);
	ASSERT_TRUE(inout->getStatus() == MOD_EXECUTION_OK);
	ASSERT_TRUE(dyinout->getStatus() == MOD_EXECUTION_OK);
	ASSERT_TRUE(inout2->getStatus() == MOD_EXECUTION_OK);
}

#endif //GROUPTEST

#endif
/*
#ifndef PYTHON_EMBEDDING_DISABLED
	TEST_F(TestSimulation,loadPythonModule) {
		ostream *out = &cout;
		DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
		DM::Logger(DM::Standard) << "Add Module";
		DM::Simulation sim;
		sim.registerPythonModules("PythonModules/scripts/");
		DM::Module * m = sim.addModule("PythonTestModule");
		ASSERT_TRUE(m != 0);
	}

	TEST_F(TestSimulation,runSinglePythonModule) {
		ostream *out = &cout;
		DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
		DM::Logger(DM::Standard) << "Add Module";
		DM::Simulation sim;
		sim.registerPythonModules("PythonModules/scripts/");
		DM::Module * m = sim.addModule("PythonTestModule");
		ASSERT_TRUE(m != 0);
		sim.run();
		ASSERT_TRUE(sim.getSimulationStatus() == DM::SIM_OK);
	}

	TEST_F(TestSimulation,runRepeatedSinglePythonModule) {
			ostream *out = &cout;
			DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
			DM::Logger(DM::Standard) << "Add Module";
			DM::Simulation sim;
			sim.registerPythonModules("PythonModules/scripts/");
			DM::Module * m = sim.addModule("PythonTestModule");
			ASSERT_TRUE(m != 0);
			for (int i = 0; i < 100; i++)
				sim.run();
			ASSERT_TRUE(sim.getSimulationStatus() == DM::SIM_OK);

		}
#endif
/**/
}
