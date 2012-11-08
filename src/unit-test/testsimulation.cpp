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
#include <dynamicinout.h>
#include <grouptest.h>
#include <dmporttuple.h>

namespace {
/*
TEST_F(TestSimulation,testMemory){
    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
    DM::Logger(DM::Standard) << "Add Module";
    DM::Simulation sim;
    sim.registerNativeModules("dynamind-testmodules");
    DM::Module * m = sim.addModule("CreateNodes");
    std::string m_uuid = m->getUuid();
    ASSERT_TRUE(m != 0);
    sim.run();
    ASSERT_TRUE(sim.getSimulationStatus() == DM::SIM_OK);
    for (int i = 0; i < 5; i++) {
        sim.removeModule(m_uuid);
		DM::Logger(DM::Standard) << "#" << i;
        m_uuid = sim.addModule("CreateNodes")->getUuid();
        sim.run();
    }
}

TEST_F(TestSimulation,addModuleToSimulationTest){
    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
    DM::Logger(DM::Standard) << "Add Module";
    DM::Simulation sim;
    sim.registerNativeModules("dynamind-testmodules");
    DM::Module * m = sim.addModule("TestModule");
    ASSERT_TRUE(m != 0);
}

TEST_F(TestSimulation,loadModuleNativeTest) {
    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
    DM::Logger(DM::Standard) << "Load Native Module";
    DM::Simulation sim;
    ASSERT_TRUE(sim.registerNativeModules("dynamind-testmodules") == true);
}

TEST_F(TestSimulation,repeatedRunTest) {
    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
    DM::Logger(DM::Standard) << "Test Repeatet Run";
    DM::Simulation sim;
    sim.registerNativeModules("dynamind-testmodules");
    DM::Module * m = sim.addModule("TestModule");
    ASSERT_TRUE(m != 0);
    for (long i = 0; i < 1000; i++) {
        sim.run();
        ASSERT_TRUE(sim.getSimulationStatus() == DM::SIM_OK);
    }

}

TEST_F(TestSimulation,linkedModulesTest) {
    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
    DM::Logger(DM::Standard) << "Test Linked Modules";
    DM::Simulation sim;
    sim.registerNativeModules("dynamind-testmodules");
    DM::Module * m = sim.addModule("TestModule");

    ASSERT_TRUE(m != 0);
    DM::Module * inout  = sim.addModule("InOut");

    ASSERT_TRUE(inout != 0);
    DM::ModuleLink * l = sim.addLink(m->getOutPort("Sewer"), inout->getInPort("Inport"));

    ASSERT_TRUE(l != 0);
    for (long i = 0; i < 10; i++){
        sim.run();
        ASSERT_TRUE(sim.getSimulationStatus() == DM::SIM_OK);
    }
}

TEST_F(TestSimulation,linkedDynamicModules) {
    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
    DM::Logger(DM::Standard) << "Test Linked Modules";
    DM::Simulation sim;
    sim.registerNativeModules("dynamind-testmodules");
    DM::Module * m = sim.addModule("TestModule");
    ASSERT_TRUE(m != 0);
    DM::Module * inout  = sim.addModule("InOut");
    ASSERT_TRUE(inout != 0);
    DM::ModuleLink * l = sim.addLink(m->getOutPort("Sewer"), inout->getInPort("Inport"));
    ASSERT_TRUE(l != 0);
    DynamicInOut * dyinout  = (DynamicInOut *)sim.addModule("DynamicInOut");
    ASSERT_TRUE(dyinout != 0);
    dyinout->addAttribute("D");
    DM::ModuleLink * l1 = sim.addLink(inout->getOutPort("Inport"), dyinout->getInPort("Inport"));
    ASSERT_TRUE(l1 != 0);
    DM::Module * inout2  = sim.addModule("InOut2");
    ASSERT_TRUE(inout2 != 0);
    DM::ModuleLink * l2 = sim.addLink(dyinout->getOutPort("Inport"), inout2->getInPort("Inport"));
    ASSERT_TRUE(l2 != 0);
    sim.run();
    ASSERT_TRUE(sim.getSimulationStatus() == DM::SIM_OK);
}

TEST_F(TestSimulation,linkedDynamicModulesOverGroups) {
    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
    DM::Logger(DM::Standard) << "Test Linked Modules";
    DM::Simulation sim;
    sim.registerNativeModules("dynamind-testmodules");
    DM::Module * m = sim.addModule("TestModule");
    ASSERT_TRUE(m != 0);
    DM::Module * inout  = sim.addModule("InOut");
    ASSERT_TRUE(inout != 0);
    DM::ModuleLink * l = sim.addLink(m->getOutPort("Sewer"), inout->getInPort("Inport"));
    ASSERT_TRUE(l != 0);
    //Here comes the group
    GroupTest * g = (GroupTest * ) sim.addModule("GroupTest");
    g->addInPort("In");
    DM::ModuleLink * l_in = sim.addLink(inout->getOutPort("Inport"),g->getInPortTuple("In")->getInPort());
    ASSERT_TRUE(l_in != 0);
    g->addOutPort("Out");
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
    sim.run();
    ASSERT_TRUE(sim.getSimulationStatus() == DM::SIM_OK);
}

TEST_F(TestSimulation,validationtool) {
    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
    DM::Logger(DM::Standard) << "Test validation tool";
    DM::Simulation sim;
    sim.registerNativeModules("dynamind-testmodules");
    DM::Module * mcreator = sim.addModule("CreateAllComponenets");
    std::string uuid = mcreator->getUuid();
    ASSERT_TRUE(mcreator != 0);
    DM::Module * mcheck  = sim.addModule("CheckAllComponenets");
    ASSERT_TRUE(mcheck != 0);
    DM::ModuleLink * l2 = sim.addLink(mcreator->getOutPort("sys"), mcheck->getInPort("sys"));
    ASSERT_TRUE(l2 != 0);
    sim.run();
    //for(int i=0;i<500;i++)
    //{
    //    DM::Logger(DM::Standard) << "run #" << i;
    //    sim.run();
    //    sim.getModuleWithUUID(uuid)->setExecuted(false);
    //}
    ASSERT_TRUE(sim.getSimulationStatus() == DM::SIM_OK);
}


TEST_F(TestSimulation,simplesqltest) {
    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
    DM::Logger(DM::Standard) << "Test Reallocation (SQL)";
    DM::Simulation sim;
    sim.registerNativeModules("dynamind-testmodules");
    DM::Module * mcreator = sim.addModule("CreateAllComponenets");
    ASSERT_TRUE(mcreator != 0);
    DM::Module * mallocator  = sim.addModule("Reallocator");
    ASSERT_TRUE(mallocator != 0);
    DM::Module * mcheck  = sim.addModule("CheckAllComponenets");
    ASSERT_TRUE(mcheck != 0);
    DM::ModuleLink * l1 = sim.addLink(mcreator->getOutPort("sys"), mallocator->getInPort("sys"));
    ASSERT_TRUE(l1 != 0);
    DM::ModuleLink * l2 = sim.addLink(mallocator->getOutPort("sys"), mcheck->getInPort("sys"));
    ASSERT_TRUE(l2 != 0);
    sim.run();
    ASSERT_TRUE(sim.getSimulationStatus() == DM::SIM_OK);
}

*/

TEST_F(TestSimulation,sqlprofiling) {
    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);
    DM::Logger(DM::Standard) << "Test Profiling (SQL)";


    QElapsedTimer timer;
    timer.start();

    for(int i=0;i<1000;i++)
    {
        //DM::Logger(DM::Standard) << "iteration "<<i;
        DM::Node* n = new DM::Node(0,0,0);
        n->setX(1.0);
        delete n;
    }

    DM::Logger(DM::Standard) << "time for single nodes: " << (long)timer.elapsed();

    timer.restart();
    DM::Node* baseNode = new DM::Node(0,0,0);
    DM::System* sys = new System();
    for(int i=0;i<1000;i++)
    {
        //DM::Logger(DM::Standard) << "iteration "<<i;
        DM::Node n(*baseNode);
        sys->addNode(n);
    }
    delete baseNode;
    delete sys;

    DM::Logger(DM::Standard) << "time for attached and copied nodes: " << (long)timer.elapsed();
}
/*
TEST_F(TestSimulation,sqlsuccessortest) {
    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
    DM::Logger(DM::Standard) << "Test Successor states (SQL)";
    DM::Simulation sim;
    sim.registerNativeModules("dynamind-testmodules");
    DM::Module * mcreator = sim.addModule("CreateAllComponenets");
    ASSERT_TRUE(mcreator != 0);
    DM::Module * msucc  = sim.addModule("SuccessorCheck");
    ASSERT_TRUE(msucc != 0);
    DM::ModuleLink * l1 = sim.addLink(mcreator->getOutPort("sys"), msucc->getInPort("sys"));
    ASSERT_TRUE(l1 != 0);
    sim.run();
    ASSERT_TRUE(sim.getSimulationStatus() == DM::SIM_OK);
}

TEST_F(TestSimulation, SqlFaceOrder)
{ 
	ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
    DM::Logger(DM::Standard) << "Test face nodes order (SQL)";

	std::vector<std::string> nodesin;
	nodesin.push_back("one");
	nodesin.push_back("two");
	nodesin.push_back("three");	
	
	std::vector<std::string> hole0in;
	hole0in.push_back("0one");
	hole0in.push_back("0two");
	hole0in.push_back("0three");
	std::vector<std::string> hole1in;
	hole1in.push_back("1one");
	hole1in.push_back("1two");
	hole1in.push_back("1three");

	DM::Face* face = new DM::Face(nodesin);
	face->addHole(hole0in);
	face->addHole(hole1in);

	std::vector<std::string> nodesout = face->getNodes();
	std::vector<std::vector<std::string> > holesout =  face->getHoles();
	    
	ASSERT_TRUE(nodesin.size() == nodesout.size());
	for(unsigned int i=0;i<nodesin.size();i++)
		ASSERT_TRUE(nodesin[i] == nodesout[i]);

	ASSERT_TRUE(holesout.size() == 2);

	for(unsigned int i=0;i<holesout[0].size();i++)
		ASSERT_TRUE(holesout[0][i] == hole0in[i]);
	
	for(unsigned int i=0;i<holesout[1].size();i++)
		ASSERT_TRUE(holesout[1][i] == hole1in[i]);

	delete face;
}
TEST_F(TestSimulation, SQLRasterdata)
{ 
	ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
    DM::Logger(DM::Standard) << "Test raster data (SQL)";

	int size = 4;
	DM::RasterData* raster = new DM::RasterData(size,size,10);
	// check no value
    DM::Logger(DM::Debug) << "checking default values";
	for(long x=0;x<size;x++)
		for(long y=0;y<size;y++)
			ASSERT_TRUE(raster->getValue(x,y) == raster->getNoValue());
	// insert
    DM::Logger(DM::Debug) << "inserting values";
	for(long x=0;x<size;x++)
		for(long y=0;y<size;y++)
			raster->setValue(x,y,x*1000+y);
	// check values
    DM::Logger(DM::Debug) << "checking values";
	for(long x=0;x<size;x++)
		for(long y=0;y<size;y++)
		{
			DM::Logger(DM::Debug) << "checking " << x << "/" << y;
			ASSERT_TRUE(raster->getValue(x,y) == x*1000+y);
		}
	delete raster;

	raster = new DM::RasterData();
	raster->setSize(size,size,10);
	// check no value
    DM::Logger(DM::Debug) << "checking default values with seperatly initialized grid";
	for(long x=0;x<size;x++)
		for(long y=0;y<size;y++)
			ASSERT_TRUE(raster->getValue(x,y) == raster->getNoValue());
	// insert
    DM::Logger(DM::Debug) << "inserting values with seperatly initialized grid";
	for(long x=0;x<size;x++)
		for(long y=0;y<size;y++)
			raster->setValue(x,y,x*1000+y);
	// check values
    DM::Logger(DM::Debug) << "checking values with seperatly initialized grid";
	for(long x=0;x<size;x++)
		for(long y=0;y<size;y++)
			ASSERT_TRUE(raster->getValue(x,y) == x*1000+y);

	delete raster;
}

TEST_F(TestSimulation, SQLattributes)
{ 
	ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
    DM::Logger(DM::Standard) << "Test attributes (SQL)";

	double dbl = 13.0;
	std::vector<double> vecDbl;
	vecDbl.push_back(dbl);
	vecDbl.push_back(dbl*2);
	vecDbl.push_back(dbl*3);

	std::string str = "peter thermometer";
	std::vector<std::string> vecStr;
	vecStr.push_back(str);
	vecStr.push_back(str+str);
	vecStr.push_back(str+str+str);
	
    DM::Logger(DM::Debug) << "checking doubles";
	// DOUBLE	
	DM::Attribute* a = new DM::Attribute("fuzzi");
	a->setDouble(dbl);
	ASSERT_TRUE(a->getDouble() == dbl);
	delete a;
	
    DM::Logger(DM::Debug) << "checking double vectors";
	// DOUBLE VECTOR
	a = new DM::Attribute("fuzzi");
	a->setDoubleVector(vecDbl);
	ASSERT_TRUE(a->getDoubleVector() == vecDbl);
	delete a;
	
    DM::Logger(DM::Debug) << "checking strings";
	// STRING
	a = new DM::Attribute("fuzzi");
	a->setString(str);
	ASSERT_TRUE(a->getString() == str);
	delete a;
	
    DM::Logger(DM::Debug) << "checking string vectors";
	// STRING VECTOR
	a = new DM::Attribute("fuzzi");
	a->setStringVector(vecStr);
	ASSERT_TRUE(a->getStringVector() == vecStr);
	delete a;
	
    DM::Logger(DM::Debug) << "checking time series";
	// TIME SERIES
	a = new DM::Attribute("fuzzi");
	a->addTimeSeries(vecStr, vecDbl);
	std::vector<double> vecDblOut;
	std::vector<std::string> vecStrOut;
	a->getTimeSeries(&vecStrOut, &vecDblOut);
	ASSERT_TRUE(vecDblOut == vecDbl);
	ASSERT_TRUE(vecStrOut == vecStr);
	delete a;
	
    DM::Logger(DM::Debug) << "checking links";
	// LINKS
	a = new DM::Attribute("fuzzi");

	std::vector<DM::LinkAttribute> links;
	DM::LinkAttribute link0 = {"0","a"};
	DM::LinkAttribute link1 = {"1","b"};
	DM::LinkAttribute link2 = {"2","c"};

	links.push_back(link0);
	links.push_back(link1);
	a->setLinks(links);
	links.push_back(link2);
	a->setLink(link2.viewname, link2.uuid);
	std::vector<DM::LinkAttribute> linksOut = a->getLinks();

	for(unsigned int i=0;i<links.size();i++)
	{
		ASSERT_TRUE(linksOut[i].uuid == links[i].uuid);
		ASSERT_TRUE(linksOut[i].viewname == links[i].viewname);
	}
	delete a;
}
/**/
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
}
