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

#include <wsdimensioning.h>

//DynaMind includes
#include <dmsystem.h>
#include <dmlogsink.h>
#include <math.h>
#include <tbvectordata.h>

//CGAL
#include <CGAL/Simple_cartesian.h>
#include <CGAL/point_generators_2.h>
#include <CGAL/Orthogonal_k_neighbor_search.h>
#include <CGAL/Search_traits_2.h>
#include <list>
#include <cmath>

//Watersupply
#include <dmepanet.h>
#include <epanetmodelcreator.h>
#include <epanetdynamindconverter.h>

//boost
#include<boost/range/numeric.hpp>

using namespace DM;

DM_DECLARE_NODE_NAME(Dimensioning,Watersupply)

Dimensioning::Dimensioning()
{   
    std::vector<DM::View> views;
    views.push_back(wsd.getCompleteView(WS::JUNCTION,DM::READ));
    views.push_back(wsd.getCompleteView(WS::PIPE,DM::MODIFY));
    views.push_back(wsd.getCompleteView(WS::RESERVOIR,DM::READ));
    this->addData("Watersupply", views);
}

void Dimensioning::run()
{

    QString dir = QDir::tempPath();
    QString inpfilename = "test.inp";
    QString rptfilename = "test.rpt";


    char inpfile[256];
    strcpy(inpfile,QString(dir+"/"+inpfilename).toStdString().c_str());
    char rptfile[256];
    strcpy(rptfile,QString(dir+"/"+rptfilename).toStdString().c_str());
    EPANETModelCreator creator;


    this->sys = this->getData("Watersupply");

    if(!EpanetDynamindConverter::createEpanetModel(this->sys, &creator, inpfile))
    {
        DM::Logger(DM::Error) << "Could not create a valid EPANET inp file";
        return;
    }

    if(!EpanetDynamindConverter::checkENRet(EPANET::ENopen(inpfile,rptfile,""))) return;
    if(!SitzenfreiDimensioning())return;
    //if(!EpanetDynamindConverter::checkENRet(EPANET::ENsaveinpfile("/tmp/designed.inp")))return;
    if(!EpanetDynamindConverter::checkENRet(EPANET::ENclose()))return;
}

bool Dimensioning::SitzenfreiDimensioning()
{
    int nnodes, nlinks;
    std::vector<int> diameter={80, 100, 125, 150, 200, 250, 300, 350, 400, 500, 600, 800, 1000, 1500, 2000, 4000, 8000};
    std::vector<double> designvelocity={0.5, 0.5, 1, 1, 1, 1, 1, 1, 1, 1.5, 1.5, 1.75, 1.75, 2, 2, 2, 2, 2};

    //Get number of nodes and links
    if(!EpanetDynamindConverter::checkENRet(EPANET::ENgetcount(EN_NODECOUNT,&nnodes)))return false;
    if(!EpanetDynamindConverter::checkENRet(EPANET::ENgetcount(EN_LINKCOUNT,&nlinks)))return false;

    DM::Logger(DM::Standard) << "Starting SitzenfreiDimensioning with " << nnodes << " nodes and " << nlinks << " links.";

    //Set all diameters to smallest available diameter
    for(int index=1; index<=nlinks; index++)
        if(!EpanetDynamindConverter::checkENRet(EPANET::ENsetlinkvalue(index,EN_DIAMETER,diameter[0])))return false;

    //Simulate model the first time
    DM::Logger(DM::Standard) << "Simulate model the first time";
    if(!EpanetDynamindConverter::checkENRet(EPANET::ENsolveH()))return false;
    if(!EpanetDynamindConverter::checkENRet(EPANET::ENsolveQ()))return false;

    //initialize design criteria for velocities for the first iteration
    std::vector<double> resV(nlinks,2*designvelocity[0]);

    //auto-design process
    DM::Logger(DM::Standard) << "Start auto design";
    int i = 0;
    while( (boost::accumulate(resV, 0)>=1) && (i < diameter.size()))
    {
        i++;
        if(!EpanetDynamindConverter::checkENRet(EPANET::ENopenH()))return false;
        if(!EpanetDynamindConverter::checkENRet(EPANET::ENinitH(1)))return false;

        //set new diameters
        for(int index=0; index<nlinks; index++)
            if(resV[index]>=designvelocity[i])
                if(!EpanetDynamindConverter::checkENRet(EPANET::ENsetlinkvalue(index+1,EN_DIAMETER,diameter[i])))return false;

        //simulate
        long int t=0;
        long int tstep=1;

        while(tstep)
        {
            if(!EpanetDynamindConverter::checkENRet(EPANET::ENrunH(&t)))return false;
            if(!EpanetDynamindConverter::checkENRet(EPANET::ENnextH(&tstep)))return false;
        }

        //extract results of simulation
        for(int index=0; index<nlinks; index++)
        {
            float val;
            if(!EpanetDynamindConverter::checkENRet(EPANET::ENgetlinkvalue(index+1,EN_VELOCITY,&val)))return false;
            resV[index]=val;
        }

        if(!EpanetDynamindConverter::checkENRet(EPANET::ENcloseH()))return false;
    }

    DM::Logger(DM::Standard) << "Auto designer needed " << i+1 << " iterations.";

    return true;
}
