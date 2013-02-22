/**
 * @file
 * @author  Michael Mair <michael.mair@uibk.ac.at>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2012  Michael Mair
 
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

#include <simulatewithepanet.h>

//DynaMind includes
#include <dmsystem.h>
#include <dmlogsink.h>
#include <math.h>
#include <tbvectordata.h>

//CGAL
//#include <CGAL/Simple_cartesian.h>
//#include <CGAL/point_generators_2.h>
//#include <CGAL/Orthogonal_k_neighbor_search.h>
//#include <CGAL/Search_traits_2.h>
#include <list>
#include <cmath>

//Watersupply
#include <dmepanet.h>
#include <epanetdynamindconverter.h>
#include <epanetmodelcreator.h>

using namespace DM;

DM_DECLARE_NODE_NAME(SimulateWithEPANET,Watersupply)

SimulateWithEPANET::SimulateWithEPANET()
{   
    this->addData("Watersupply", wsd.getAll(DM::READ));
}

void SimulateWithEPANET::run()
{
    char inpfile[] = "/tmp/test.inp";
    char rptfile[] = "/tmp/test.rpt";
    EPANETModelCreator creator;


    this->sys = this->getData("Watersupply");

    if(!EpanetDynamindConverter::createEpanetModel(this->sys, &creator, inpfile))
    {
        DM::Logger(DM::Error) << "Could not create a valid EPANET inp file";
        return;
    }

    if(!EpanetDynamindConverter::checkENRet(EPANET::ENopen(inpfile,rptfile,""))) return;
    if(!EpanetDynamindConverter::checkENRet(EPANET::ENopenH()))return;
    if(!EpanetDynamindConverter::checkENRet(EPANET::ENsolveH()))return;
    if(!EpanetDynamindConverter::checkENRet(EPANET::ENclose()))return;
}
