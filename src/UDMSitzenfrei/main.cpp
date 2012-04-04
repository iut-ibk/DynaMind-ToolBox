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
#include <dmdatabase.h>
#include <dmlog.h>
#include <dmdatamanagement.h>
#include <QThreadPool>
#include <dmmodule.h>
#include <dm.h>
#include <sstream>
#include <dmpythonenv.h>
#include <QStringList>
#include <dmgroup.h>
#include <dmporttuple.h>

using namespace std;
using namespace DM;
/**
  * @addtogroup Papers
  */

/**
 * @ingroup Papers
 * @brief Simulation for the UDM2012 Paper Sitzenfrei et. al.
 * paramter for the model
 * Maxstrahler, StrahlerDifferenz, MaxStrahlerStorage, StrahlerDifferenzStorage
 * Paramter have to be set as tuple, Strahler order and probability. No sopace between sequence. 0,100 Means Strahler 0, 100%
 * MaxStrahler=0,100,1,0,2,0 StrahlerDifferenz=1,0,2,0,3,0 MaxStrahlerStorage=0,100,1,0,2,0 StrahlerDifferenzStorage=1,0,2,0,3,0
 *
 **/
DM::Simulation * createBasicSim() {
    //Init Logger
    ostream *out = &cout;
    DM::Log::init(new OStreamLogSink(*out), DM::Debug);
    DM::Logger(DM::Debug) << "Start";

    DM::PythonEnv *env = DM::PythonEnv::getInstance();
    //env->addPythonPath("/home/christian/Documents/DynaMind/build/release/");


    DataManagement::init();
    DMDatabase * db = new DMDatabase();
    DataManagement::getInstance().registerDataBase(db);   //Init Logger
    Simulation * sim =  new Simulation;
    sim->loadSimulation("testmodels/sitzenfrei_udm.dyn");




    double maxCPopDensity_max = 200;
    double maxDCPopDensity_max = 100;
    double maxOBPopDensity_max = 30;
    double popCUFRand_max = 10;
    double popDCUFRand_max = 11;
    double popAGRIRand_max = 19;
    int Steps_max = 16;


    double maxCPopDensity_min = 100;
    double maxDCPopDensity_min = 40;
    double maxOBPopDensity_min = 15;
    double popCUFRand_min = 5;
    double popDCUFRand_min = 12;
    double popAGRIRand_min = 15;
    int Steps_min = 9;



    double maxCPopDensity = (int) maxCPopDensity_min + rand() % (int) (maxCPopDensity_max-maxCPopDensity_min)+1;
    double maxDCPopDensity= (int) maxDCPopDensity_min + rand() %  (int)  (maxDCPopDensity_max-maxDCPopDensity_min)+1;
    double maxOBPopDensity= (int) maxOBPopDensity_min + rand() %  (int)  (maxOBPopDensity_max-maxOBPopDensity_min)+1;
    double popCUFRand= (int) popCUFRand_min + rand() %  (int)  (popCUFRand_max-popCUFRand_min)+1;
    double popDCUFRand= (int) popDCUFRand_min + rand() %  (int)  (popDCUFRand_max-popDCUFRand_min)+1;
    double popAGRIRand= (int) popAGRIRand_min + rand() % (int)   (popAGRIRand_max-popAGRIRand_min)+1;
    int Steps =   (int) Steps_min + rand() % (int)   (Steps_max-Steps_min)+1;
    int PopSteps = 3;
    int InitialCityCenter = rand() % 3 + 1;

    DM::Module * m = sim->getModuleByName("VIBe");


    m->setParameterValue("maxCPopDensity",  QString::number(maxCPopDensity).toStdString());
    m->setParameterValue("maxDCPopDensity",  QString::number(maxDCPopDensity).toStdString());
    m->setParameterValue("maxOBPopDensity",  QString::number(maxOBPopDensity).toStdString());
    m->setParameterValue("popCUFRand", QString::number(popCUFRand).toStdString());
    m->setParameterValue("popDCUFRand",  QString::number(popDCUFRand).toStdString());
    m->setParameterValue("popAGRIRand",  QString::number(popAGRIRand).toStdString());
    m->setParameterValue("InitialCityCenter",  QString::number(InitialCityCenter).toStdString());
    m->setParameterValue("PopSteps",  QString::number(PopSteps).toStdString());
    m->setParameterValue("Steps",  QString::number(Steps).toStdString());

    m = sim->getModuleByName("catchments");
    m->setParameterValue("Height", "200");
    m->setParameterValue("Width", "200");

    return sim;
}

int main(int argc, char *argv[]) {

    std::vector<std::string> Scenarios;
    std::string secenario1 = "MaxStrahler=0,0 StrahlerDifferenz=1,0 MaxStrahlerStorage=0,0 StrahlerDifferenzStorage=1,0";

    std::string secenario2 = "MaxStrahler=0,100 StrahlerDifferenz=1,0 MaxStrahlerStorage=0,0 StrahlerDifferenzStorage=1,0";
    std::string secenario3 = "MaxStrahler=0,100 StrahlerDifferenz=1,0 MaxStrahlerStorage=0,100 StrahlerDifferenzStorage=1,0";

    std::string secenario4 = "MaxStrahler=1,100 StrahlerDifferenz=1,0 MaxStrahlerStorage=0,0 StrahlerDifferenzStorage=1,0";
    std::string secenario5 = "MaxStrahler=1,100 StrahlerDifferenz=1,0 MaxStrahlerStorage=1,100 StrahlerDifferenzStorage=1,0";

    std::string secenario6 = "MaxStrahler=2,100 StrahlerDifferenz=1,0 MaxStrahlerStorage=0,0 StrahlerDifferenzStorage=1,0";
    std::string secenario7 = "MaxStrahler=2,100 StrahlerDifferenz=1,0 MaxStrahlerStorage=2,100 StrahlerDifferenzStorage=1,0";

    std::string secenario8 = "MaxStrahler=3,100 StrahlerDifferenz=1,0 MaxStrahlerStorage=0,0 StrahlerDifferenzStorage=1,0";
    std::string secenario9 = "MaxStrahler=3,100 StrahlerDifferenz=1,0 MaxStrahlerStorage=3,100 StrahlerDifferenzStorage=1,0";

    std::string secenario10 = "MaxStrahler=10,100 StrahlerDifferenz=1,0 MaxStrahlerStorage=0,0 StrahlerDifferenzStorage=1,0";
    std::string secenario11 = "MaxStrahler=10,100 StrahlerDifferenz=1,0 MaxStrahlerStorage=10,100 StrahlerDifferenzStorage=1,0";

    Scenarios.push_back(secenario1);
    Scenarios.push_back(secenario2);
    Scenarios.push_back(secenario3);
    Scenarios.push_back(secenario4);
    Scenarios.push_back(secenario5);
    Scenarios.push_back(secenario6);
    Scenarios.push_back(secenario7);
    Scenarios.push_back(secenario8);
    Scenarios.push_back(secenario9);
    Scenarios.push_back(secenario10);
    Scenarios.push_back(secenario11);

    DM::Simulation * sim = createBasicSim();
    DM::Group * ScenarioGroup = (DM::Group *) sim->getModuleByName("SewerGeneration");
    DM::Module * linkScenarioGroup = sim->getModuleByName("Network");

    foreach (std::string scenario, Scenarios) {


        //Add scenario to simulation
        sim->loadSimulation("testmodels/sitzenfrei_scenario_udm.dyn");

        DM::Group * g =(DM::Group*)sim->getModuleByName("Scenario");
        //g->setGroup(ScenarioGroup);
        g->setName("");

        //Link Module
        sim->addLink(ScenarioGroup->getOutPortTuple("City")->getOutPort(), g->getInPortTuple("City")->getInPort());

        QStringList ParameterList = QString::fromStdString(scenario).split(" ");
        //first parameternot needed



        std::map<int, int> MaxStrahler;
        std::map<int, int> StrahlerDifferenz;
        std::map<int, int> MaxStrahlerStorage;
        std::map<int, int> StrahlerDifferenzStorage;


        QStringList parameter;
        QStringList filename;
        parameter << "MaxStrahler" << "StrahlerDifferenz" << "MaxStrahlerStorage" << "StrahlerDifferenzStorage";
        foreach (QString line, ParameterList) {
            QStringList splitline = line.split("=");
            if (splitline[0] == "MaxStrahler") {
                if (splitline.size() > 1) {
                    QStringList values = splitline[1].split(",");
                    for (int i = 0; i < values.size(); i++) {
                        MaxStrahler[values[i].toInt()] = values[i+1].toInt();
                        i++;
                    }

                }
                filename << line;
            }
            if (splitline[0] == "StrahlerDifferenz") {
                if (splitline.size() > 1) {
                    QStringList values = splitline[1].split(",");
                    for (int i = 0; i < values.size(); i++) {
                        StrahlerDifferenz[values[i].toInt()] = values[i+1].toInt();
                        i++;
                    }

                }
                filename << line;
            }
            if (splitline[0] == "MaxStrahlerStorage") {
                if (splitline.size() > 1) {
                    QStringList values = splitline[1].split(",");
                    for (int i = 0; i < values.size(); i++) {
                        MaxStrahlerStorage[values[i].toInt()] = values[i+1].toInt();
                        i++;
                    }

                }
                filename << line;
            }
            if (splitline[0] == "StrahlerDifferenzStorage") {
                if (splitline.size() > 1) {
                    QStringList values = splitline[1].split(",");
                    for (int i = 0; i < values.size(); i++) {
                        StrahlerDifferenzStorage[values[i].toInt()] = values[i+1].toInt();
                        i++;
                    }

                }
                filename << line;
            }
        }





        DM::Module * m = sim->getModuleByName("Outfall");
        m->setName("");




        m->setParameterValue("StrahlerDifferenz", DMHelper::convertIntMapToDMMapString(StrahlerDifferenz));
        m->setParameterValue("MaxStrahler", DMHelper::convertIntMapToDMMapString(MaxStrahler));

        m->setParameterValue("StrahlerDifferenzStorage", DMHelper::convertIntMapToDMMapString(StrahlerDifferenzStorage));
        m->setParameterValue("MaxStrahlerStorage", DMHelper::convertIntMapToDMMapString(MaxStrahlerStorage));

        m = sim->getModuleByName("Export");
        m->setName("");
        filename << "new_new.ress";
        filename.join("");
        m->setParameterValue("FileName", filename.join("").toStdString());
    }

    for (int i = 0; i < 100; i++)
        sim->startSimulation(true);


    sim->startSimulation();

    //delete sim;

}

