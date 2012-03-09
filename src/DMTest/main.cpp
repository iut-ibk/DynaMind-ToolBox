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
#include <complexgeometry.h>

using namespace std;
using namespace DM;

bool DynaMiteTest();
bool DMBaseTest();
bool MemDynaMiteTestPython();
bool MemDynaMiteTestC();
bool StoryLine();
int main(int argc, char *argv[], char *envp[]) {
    //Init Logger
    ostream *out = &cout;
    DM::Log::init(new OStreamLogSink(*out), DM::Debug);
    DM::Logger(DM::Debug) << "Start";

    /*if(!DynaMiteTest())
        Logger(Error) << "DynaMiteTest FAILED";
    else
        Logger() << "DynaMiteTest DONE";

    if(!DMBaseTest())
        Logger(Error) << "DMBaseTest FAILED";
    else
        Logger() << "DMBaseTest DONE";

    if(!DMBaseTest())
        Logger(Error) << "DMBaseTest_2 FAILED";
    else
        Logger() << "DMBaseTest_2 DONE";

    if(!DMBaseTest())
        Logger(Error) << "DMBaseTest_2 FAILED";
    else
        Logger() << "DMBaseTest_2 DONE";

    Logger() << "ComplexGeomtry START";
    if (!ComplexGeometry()) {
        Logger(Error) << "ComplexGeomtry FAILED";

    }else {
        Logger() << "ComplexGeomtry DONE";
    }

    if (!StoryLine()) {
        Logger(Error) << "StoryLine FAILED";

    }else {
        Logger() << "StoryLine DONE";
    }*/
    if (!MemDynaMiteTestPython()) {
        Logger(Error) << "MemDynaMiteTest FAILED";

    }else {
        Logger() << "MemDynaMiteTest DONE";
    }
    return 1;
}
bool DMBaseTest() {
    DM::System * s = new DM::System("test");
    DM::Node * n = s->addNode(0,0,0);
    n->addAttribute(DM::Attribute("a"));
    n->addAttribute(DM::Attribute("b"));
    std::string name = n->getName();


    s = s->createSuccessor();
    n = s->getNode(name);
    n->addAttribute(DM::Attribute("c"));

    for (std::map<std::string, DM::Attribute*>::const_iterator it = n->getAllAttributes().begin(); it != n->getAllAttributes().end(); ++it) {
        std::cout << it->first << std::endl;


    }
    return true;

}

bool DynaMiteTest()
{
    DM::PythonEnv *env = DM::PythonEnv::getInstance();
    //env->addPythonPath("/home/csae6550/work/VIBe2Core/build/Release/");
    env->addPythonPath("/home/c8451045/Documents/DynaMind/build/debug/");


    DataManagement::init();
    DMDatabase * db = new DMDatabase();
    DataManagement::getInstance().registerDataBase(db);   //Init Logger
    Simulation * sim = new Simulation;
    sim->registerNativeModules("dmtestmodule");
    sim->registerPythonModules("/home/c8451045/Documents/DynaMind/scripts");
    DM::Module * in = sim->addModule("TestModule");
    DM::Module * outm =sim->addModule("InOut");
    DM::Module * outm2 =sim->addModule("InOut");
    DM::Module * outm3 = sim->addModule("WhiteNoise");
    DM::Module * doraster = sim->addModule("DoStuffWithRasterData");

    //vibens::Module * outm4 = sim->addModule("ImportShapeFile");
    //sim->addLink(outm4->getOutPort("Network"),outm->getInPort("Inport"));
    sim->addLink(in->getOutPort("Sewer"), outm->getInPort("Inport"));
    sim->addLink(outm->getOutPort("Inport"), outm2->getInPort("Inport"));
    sim->addLink(in->getOutPort("Sewer"),outm3->getInPort("Inport"));
    sim->addLink(in->getOutPort("RasterData"), doraster->getInPort("RasterData"));

    sim->run();




    QThreadPool::globalInstance()->waitForDone();

    sim->run(true);




    QThreadPool::globalInstance()->waitForDone();
    delete sim;
    DM::Logger(DM::Debug) << "End";

    return true;
}

bool MemDynaMiteTestC()
{
    DM::PythonEnv *env = DM::PythonEnv::getInstance();
    env->addPythonPath("/home/c8451045/Documents/DynaMind/build/debug/");


    DataManagement::init();
    DMDatabase * db = new DMDatabase();
    DataManagement::getInstance().registerDataBase(db);   //Init Logger
    Simulation * sim = new Simulation;
    sim->registerNativeModules("dmtestmodule");
    DM::Module * in = sim->addModule("MemoryTest");
    sim->run();




    QThreadPool::globalInstance()->waitForDone();

    sim->run(true);




    QThreadPool::globalInstance()->waitForDone();
    delete sim;
    DM::Logger(DM::Debug) << "End";

    return true;
}

bool StoryLine()
{
    DM::PythonEnv *env = DM::PythonEnv::getInstance();
    env->addPythonPath("/home/c8451045/Documents/DynaMind/build/debug/");


    DataManagement::init();
    DMDatabase * db = new DMDatabase();
    DataManagement::getInstance().registerDataBase(db);   //Init Logger
    Simulation * sim = new Simulation;
    sim->registerNativeModules("dynamindsewer");
    sim->registerPythonModules("/home/c8451045/Documents/DynaMind/scripts");
    //DM::Module * in = sim->addModule("MemTestSystem");
    DM::Module * in = sim->addModule("ImportShapeFile");
    std::vector<DM::View> views;
    DM::View conduit("CONDUIT", DM::EDGE, DM::MODIFY);

    views.push_back(conduit);
    in->addData("Vec", views);
    //in->setParameterValue("FileName", "/home/c8451045/Documents/GIS Data/drainagedata/Drains.shp");
    in->setParameterValue("FileName", "/home/c8451045/Documents/GIS Data/Scotchmans Creek Drainage/drain_1.shp");
    //in->setParameterValue("FileName","/home/c8451045/Documents/DynaMind/build/debug/Shapefile_lines.shp");

    DM::View outlet("OUTLET", DM::NODE, DM::WRITE);

    DM::Module * in1 = sim->addModule("ImportShapeFile");
    views.clear();
    views.push_back(outlet);
    in1->addData("Vec", views);

    in1->setParameterValue("FileName", "/home/c8451045/Documents/GIS Data/Scotchmans Creek Drainage/Outlets.shp");

    DM::Module * directNetwork = sim->addModule("DirectNetwork");

    DM::Module * netan = sim->addModule("NetworkAnalysis");





    sim->addLink(in1->getOutPort("Vec"), in->getInPort("Vec"));
    sim->addLink(in->getOutPort("Vec"), directNetwork->getInPort("Vec"));
    sim->addLink(directNetwork->getOutPort("Vec"), netan->getInPort("City"));

    DM::Module * out = sim->addModule("ExportToShapeFile");
    //out->setParameterValue("FileName", );

    sim->addLink(netan->getOutPort("City"), out->getInPort("vec"));





    DM::Module * plot = sim->addModule("PlotVectorData");
    sim->addLink(netan->getOutPort("City"), plot->getInPort("Vec"));

    sim->run();
    QThreadPool::globalInstance()->waitForDone();
    delete sim;
    DM::Logger(DM::Debug) << "End";

    return true;
}

bool MemDynaMiteTestPython()
{
    DM::PythonEnv *env = DM::PythonEnv::getInstance();
    env->addPythonPath("/home/c8451045/Documents/DynaMind/build/debug/");


    DataManagement::init();
    DMDatabase * db = new DMDatabase();
    DataManagement::getInstance().registerDataBase(db);   //Init Logger
    Simulation * sim = new Simulation;
    sim->registerNativeModules("dynamindsewer");
    sim->registerPythonModules("/home/c8451045/Documents/DynaMind/scripts");
    //DM::Module * in = sim->addModule("MemTestSystem");
    DM::Module * in = sim->addModule("ImportShapeFile");
    std::vector<DM::View> views;
    DM::View conduit("CONDUIT", DM::EDGE, DM::MODIFY);

    views.push_back(conduit);
    in->addData("Vec", views);
    //in->setParameterValue("FileName", "/home/c8451045/Documents/GIS Data/drainagedata/Drains.shp");
    in->setParameterValue("FileName", "/home/c8451045/Documents/GIS Data/Scotchmans Creek Drainage/drain_1.shp");
    //in->setParameterValue("FileName","/home/c8451045/Documents/DynaMind/build/debug/Shapefile_lines.shp");

    DM::View outlet("OUTLET", DM::NODE, DM::WRITE);

    DM::Module * in1 = sim->addModule("ImportShapeFile");
    views.clear();
    views.push_back(outlet);
    in1->addData("Vec", views);

    in1->setParameterValue("FileName", "/home/c8451045/Documents/GIS Data/Scotchmans Creek Drainage/Outlets.shp");


    DM::Module * plot = sim->addModule("PlotVectorData");
    sim->addLink(in1->getOutPort("Vec"), in->getInPort("Vec"));
    sim->addLink(in->getOutPort("Vec"), plot->getInPort("Vec"));


    sim->run();
    QThreadPool::globalInstance()->waitForDone();
    delete sim;
    DM::Logger(DM::Debug) << "End";

    return true;
}
