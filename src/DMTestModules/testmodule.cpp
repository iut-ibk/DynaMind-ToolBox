/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of VIBe2
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
#include "rasterdata.h"
#include <iostream>
//#include <omp.h>
#include <cmath>
#include <iostream>
#include <DMcomponent.h>
#include <DMedge.h>
#include <DMattribute.h>
#include <DMnode.h>
#include <DMview.h>

VIBe_DECLARE_NODE_NAME( TestModule,Modules )
TestModule::TestModule() {

    DM::View inlets = DM::View("Inlets");
    inlets.addComponent(DM::NODE);
    inlets.addAttributes("A");
    inlets.addAttributes("B");
    DM::View conduits = DM::View("Conduits");
    conduits.addComponent(DM::EDGE);

    std::vector<DM::View> views;

    views.push_back(inlets);
    views.push_back(conduits);


    this->addData("Sewer",views, &outputData);



}



void TestModule::run() {

    DM::Node * n1 = outputData->addNode(0,0,0, "Inlets");
    DM::Node * n2 = outputData->addNode(0,0,1, "Inlets");

    outputData->addEdge(n1, n2, "Conduits");

    Logger(Debug) << "Run Testmodule";



}

TestModule::~TestModule() {
    Logger(Debug) << "Destructor Testmodule";

}
