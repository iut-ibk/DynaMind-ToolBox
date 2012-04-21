/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2011  Christian Urich

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
#include "testmodule.h"
#include "dmrasterdata.h"
#include <iostream>
//#include <omp.h>
#include <cmath>
#include <iostream>
#include <dmcomponent.h>
#include <dmedge.h>
#include <dmattribute.h>
#include <dmnode.h>
#include <dmview.h>



DM_DECLARE_NODE_NAME( TestModule,Modules )
TestModule::TestModule() {

    inlets = DM::View("Inlets", DM::NODE, DM::WRITE);
    inlets.addAttribute("A");
    inlets.addAttribute("B");
    conduits = DM::View("Conduits", DM::EDGE, DM::WRITE);
    SomeRandomInformation = DM::View("SomeRandomInformation", DM::EDGE, DM::WRITE);
    rdata = DM::View("RasterData", DM::RASTERDATA, DM::WRITE);

    std::vector<DM::View> views;

    views.push_back(inlets);
    views.push_back(conduits);
    views.push_back(SomeRandomInformation);

    std::vector<DM::View> rdataviews;
    rdataviews.push_back(rdata);

    value = 10;




    this->addParameter("DoubleValue", DM::DOUBLE, &value);


    this->addData("Sewer",views);

    this->addData("RasterData",rdataviews);
}



void TestModule::run() {
    outputData = this->getData("Sewer");
    DM::Node * n1 = outputData->addNode(0,0,0, inlets);
    DM::Node * n2 = outputData->addNode(0,0,1, inlets);

    outputData->addEdge(n1, n2, conduits);

    outputData->addEdge(n1, n2, SomeRandomInformation);

    outputRasterData = this->getRasterData("RasterData", rdata);

    outputRasterData->setSize(200, 200, 20);
    for (int i = 0; i < outputRasterData->getWidth(); i++) {
        for (int j = 0; j < outputRasterData->getHeight(); j++) {
            outputRasterData->setValue(i,j,15);
        }

    }
    Logger(Debug) << "Run Testmodule";



}

TestModule::~TestModule() {
    Logger(Debug) << "Destructor Testmodule";

}
