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

//QT
#include <QDir>

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

	this->fixeddiameters=false;
	this->pipestatus=true;
	this->maxdiameter=1000;
	this->addParameter("Use predefined diameters", DM::BOOL, &this->fixeddiameters);
	this->addParameter("Maximum diameter [mm]", DM::DOUBLE, &this->maxdiameter);
	this->addParameter("Automatic set pipe status", DM::BOOL, &this->pipestatus);
}

void Dimensioning::run()
{

	QString dir = QDir::tempPath();
	std::string inpfilename = dir.toStdString() + "/test.inp";
	std::string rptfilename = dir.toStdString() + "/test.rpt";

	converter = boost::make_shared<EpanetDynamindConverter>();

	this->sys = this->getData("Watersupply");

	if(!converter->createEpanetModel(this->sys,inpfilename))
	{
		DM::Logger(DM::Error) << "Could not create a valid EPANET inp file";
		return;
	}

	if(!converter->openEpanetModel(inpfilename,rptfilename)) return;
	if(!SitzenfreiDimensioning())return;
	if(!converter->mapEpanetAttributes(this->sys)) return;
	//if(!converter->checkENRet(EPANET::ENsaveinpfile("/tmp/designed.inp")))return;
	converter->closeEpanetModel();
}

bool Dimensioning::SitzenfreiDimensioning()
{
	int nnodes, nlinks;
	int diameter[] = {80, 100, 125, 150, 200, 250, 300, 350, 400, 500, 600, 800, 1000, 1500, 2000, 4000, 8000};
	double designvelocity[] = {0.5, 0.5, 1, 1, 1, 1, 1, 1, 1, 1.5, 1.5, 1.75, 1.75, 2, 2, 2, 2, 2};
	std::vector<uint> fixedpipes;

	//Get number of nodes and links
	if(!converter->checkENRet(EPANET::ENgetcount(EN_NODECOUNT,&nnodes)))return false;
	if(!converter->checkENRet(EPANET::ENgetcount(EN_LINKCOUNT,&nlinks)))return false;

	DM::Logger(DM::Standard) << "Starting SitzenfreiDimensioning with " << nnodes << " nodes and " << nlinks << " links.";

	//Search for fixed pipe diamters and initialize all diameters with smallest diameter from diameter vector (fixed is if diameter > 0)
	for(int index=0; index < nlinks; index++)
	{
		float currentdiameter = 0;
		//Index in epanet always +1 (They start counting from 1 [Thumb up :-)])
		if(!converter->checkENRet(EPANET::ENgetlinkvalue(index+1,EN_DIAMETER,&currentdiameter)))return false;

		if(currentdiameter > 15 )
		{
			if(this->fixeddiameters)
				fixedpipes.push_back(index);
		}
		else
			if(!converter->checkENRet(EPANET::ENsetlinkvalue(index+1,EN_DIAMETER,diameter[0])))return false;

		if(pipestatus)
		{
			int type;
			if(!converter->checkENRet(EPANET::ENgetlinktype(index+1,&type)))return false;
			if(type!=EN_CVPIPE)
			{
				if(!converter->checkENRet(EPANET::ENsetlinkvalue(index+1,EN_INITSTATUS,1.0)))return false;
			}
		}
	}

	//Simulate model the first time
	DM::Logger(DM::Standard) << "Simulate model the first time";
	if(!converter->checkENRet(EPANET::ENsolveH()))return false;
	//if(!converter->checkENRet(EPANET::ENsolveQ()))return false;

	//initialize design criteria for velocities for the first iteration
	std::vector<double> resV(nlinks,2*designvelocity[0]);

	//auto-dimensioning process according to designevelocity
	DM::Logger(DM::Standard) << "Start auto dimensioning";
	int i = 0;
	while( (boost::accumulate(resV, 0)>=1) && (i < (sizeof(diameter)/sizeof(int)-1)))
	{
		i++;
		DM::Logger(DM::Standard) << "Current diameter: " << diameter[i];
		if(!converter->checkENRet(EPANET::ENopenH()))return false;
		if(!converter->checkENRet(EPANET::ENinitH(1)))return false;

		//set new diameters
		for(int index=0; index<nlinks; index++)
			if(resV[index] >= designvelocity[i])
				if(std::find(fixedpipes.begin(),fixedpipes.end(),index)==fixedpipes.end())  //If the diameter is forced it often results in an model with negativ pressure
					if(diameter[i] <= this->maxdiameter)
						if(!converter->checkENRet(EPANET::ENsetlinkvalue(index+1,EN_DIAMETER,diameter[i])))return false;

		//simulate
		long int t=0;
		long int tstep=1;

		while(tstep)
		{
			EPANET::ENrunH(&t);
			EPANET::ENnextH(&tstep);
		}

		//extract results of simulation
		for(int index=0; index<nlinks; index++)
		{
			float val;
			if(!converter->checkENRet(EPANET::ENgetlinkvalue(index+1,EN_VELOCITY,&val)))return false;
			resV[index]=val;
		}

		if(!converter->checkENRet(EPANET::ENcloseH()))return false;
	}

	//auto-dimensioning process according to headloss
	while( (boost::accumulate(resV, 0)>=1) && (i-(sizeof(diameter)/sizeof(int)) < (sizeof(diameter)/sizeof(int))))
	{
		i++;
		if(!converter->checkENRet(EPANET::ENopenH()))return false;
		if(!converter->checkENRet(EPANET::ENinitH(1)))return false;
		//set new diameters
		for(int index=0; index<nlinks; index++)
			if(std::find(fixedpipes.begin(),fixedpipes.end(),index)!=fixedpipes.end())
			{
				float headloss = 0;
				float fixeddiameter = 0;
				float pipelength = 0;
				if(!converter->checkENRet(EPANET::ENgetlinkvalue(index+1,EN_HEADLOSS,&headloss)))return false;
				if(!converter->checkENRet(EPANET::ENgetlinkvalue(index+1,EN_LENGTH,&pipelength)))return false;
				if(!converter->checkENRet(EPANET::ENgetlinkvalue(index+1,EN_DIAMETER,&fixeddiameter)))return false;

				if((headloss/pipelength)*1000 > 20.0)
				{
					uint d = 1;
					while(fixeddiameter > diameter[d-1] && d <= sizeof(diameter)/sizeof(int)-1)d++;

					if(!converter->checkENRet(EPANET::ENsetlinkvalue(index+1,EN_DIAMETER,diameter[d])))return false;
				}
			}

		//simulate
		long int t=0;
		long int tstep=1;

		while(tstep)
		{
			if(!converter->checkENRet(EPANET::ENrunH(&t)))return false;
			if(!converter->checkENRet(EPANET::ENnextH(&tstep)))return false;
		}

		//extract results of simulation
		for(int index=0; index<nlinks; index++)
		{
			float val;
			if(!converter->checkENRet(EPANET::ENgetlinkvalue(index+1,EN_VELOCITY,&val)))return false;
			resV[index]=val;
		}

		if(!converter->checkENRet(EPANET::ENcloseH()))return false;
	}

	DM::Logger(DM::Standard) << "Auto dimensioning needed " << i+1 << " iterations.";

	return true;
}
