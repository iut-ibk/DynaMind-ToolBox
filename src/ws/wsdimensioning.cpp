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
	this->fixeddiameters=false;
	this->pipestatus=true;
	this->maxdiameter=1000;
	this->usemainpipe=false;
	this->usereservoirdata=false;
	this->discrete=true;
	this->iterations = 5;
	this->nearestdiscretediameter=true;
	this->apprdt = 0.005;
	this->addParameter("Use predefined diameters", DM::BOOL, &this->fixeddiameters);
	this->addParameter("Maximum diameter [mm]", DM::DOUBLE, &this->maxdiameter);
	this->addParameter("Automatic set pipe status", DM::BOOL, &this->pipestatus);
	this->addParameter("Use main pipe data",DM::BOOL, &this->usemainpipe);
	this->addParameter("Use reservoirs data", DM::BOOL, &this->usereservoirdata);
	this->addParameter("Number of iterations", DM::DOUBLE, &this->iterations);
	this->addParameter("Force discrete diameters", DM::BOOL, &this->discrete);
	this->addParameter("Force nearest discrete diameters", DM::BOOL, &this->nearestdiscretediameter);
	this->addParameter("Assumed pressure head loss [m/m]", DM::DOUBLE, &this->apprdt);
}

void Dimensioning::init()
{
	std::vector<DM::View> views;
	views.push_back(wsd.getCompleteView(WS::JUNCTION,DM::READ));
	views.push_back(wsd.getCompleteView(WS::PIPE,DM::MODIFY));
	views.push_back(wsd.getCompleteView(WS::RESERVOIR,DM::READ));

	if(usemainpipe)
		views.push_back(wsd.getCompleteView(WS::MAINPIPE,DM::READ));

	this->addData("Watersupply", views);
}

void Dimensioning::run()
{
	QString dir = QDir::tempPath();
	std::string inpfilename = dir.toStdString() + "/test.inp";
	std::string rptfilename = dir.toStdString() + "/test.rpt";
	DM::Logger(DM::Standard) << "Writing file: " << inpfilename;

	if(usemainpipe)
		fixeddiameters=true;

	converter = boost::make_shared<EpanetDynamindConverter>();

	this->sys = this->getData("Watersupply");
	double totaldemand = calcTotalDemand();
	std::vector<DM::Edge*> entrypipes;

	if(usemainpipe)
		if(!approximateMainPipes(usereservoirdata,totaldemand,entrypipes,discrete))
			return;

	if(!converter->createEpanetModel(this->sys,inpfilename))
	{
		DM::Logger(DM::Error) << "Could not create a valid EPANET inp file";
		return;
	}

	if(!converter->openEpanetModel(inpfilename,rptfilename)) return;

	//set small simulation duration and timesteps
	EPANET::ENsettimeparam(EN_DURATION,60);
	EPANET::ENsettimeparam(EN_HYDSTEP,30);

	if(!SitzenfreiDimensioning())return;

	if(usemainpipe)
	{
		for (int var = 0; var < this->iterations; ++var)
		{
			if(usereservoirdata)
				if(!calibrateReservoirOutFlow(totaldemand, 100,entrypipes,false))
					return;

			DM::Logger(DM::Standard) << "Calibrate reservoir out flow .... DONE";

			if(!approximatePressure(discrete))
				return;

			DM::Logger(DM::Standard) << "Approximate pressure .... DONE";

			if(!approximatePipeSizes(true,discrete))
				return;

			DM::Logger(DM::Standard) << "Approximate pipe sizes .... DONE";
		}

		if(discrete)
			if(!approximatePipeSizes(true,discrete))
				return;
	}

	if(!converter->mapEpanetAttributes(this->sys)) return;
	//if(!converter->checkENRet(EPANET::ENsaveinpfile("/home/csae6550/Desktop/designed.inp")))return;
	converter->closeEpanetModel();

	DM::Logger(DM::Standard) << "Total demand: " << totaldemand << " m^3/s";

	QFile::remove(QString::fromStdString(inpfilename));
	QFile::remove(QString::fromStdString(rptfilename));
}

double Dimensioning::calcTotalDemand()
{
	double sum = 0;
	std::vector<DM::Component*> junctions = this->sys->getAllComponentsInView(wsd.getCompleteView(WS::JUNCTION,DM::READ));

	for(int index=0; index<junctions.size(); index++)
		sum += junctions[index]->getAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Demand))->getDouble();

	return sum/1000.0;
}

std::vector<DM::Node*> Dimensioning::getInverseFlowNeighbours(DM::Node* junction)
{
	std::vector<DM::Node*> result;
	std::vector<DM::Edge*> edgevec = junction->getEdges();

	for(int index=0; index < edgevec.size(); index++)
	{
		DM::Edge* currentedge = edgevec[index];
		int ID = converter->getEpanetLinkID(currentedge);

		if(ID < 0)
			continue;

		bool invert=true;

		if(currentedge->getStartNode()==junction)
			invert=false;

		float flow = 0.0;

		if(!converter->checkENRet(EPANET::ENgetlinkvalue(ID,EN_FLOW,&flow)))return std::vector<DM::Node*>();

		if(std::fabs(flow) < 0.001)
			flow=0.0;

		if(invert)
			flow = -flow;

		if(flow <= 0)
			continue;

		if(currentedge->getStartNode()==junction)
			result.push_back(currentedge->getEndNode());
		else
			result.push_back(currentedge->getStartNode());
	}

	return result;
}

std::vector<DM::Node*> Dimensioning::getFlowNeighbours(DM::Node* junction)
{
	std::vector<DM::Node*> result;
	std::vector<DM::Edge*> edgevec = junction->getEdges();

	for(int index=0; index < edgevec.size(); index++)
	{
		DM::Edge* currentedge = edgevec[index];
		int ID = converter->getEpanetLinkID(currentedge);

		if(ID < 0)
			continue;

		bool invert=true;

		if(currentedge->getStartNode()==junction)
			invert=false;

		float flow = 0.0;

		if(!converter->checkENRet(EPANET::ENgetlinkvalue(ID,EN_FLOW,&flow)))
			return std::vector<DM::Node*>();

		if(std::fabs(flow) < 0.01)
			flow=0.0;

		if(invert)
			flow = -flow;

		if(flow >= 0.0)
			continue;

		if(currentedge->getStartNode()==junction)
			result.push_back(currentedge->getEndNode());
		else
			result.push_back(currentedge->getStartNode());
	}

	return result;
}

DM::Node* Dimensioning::getNearestPressure(DM::Node* currentpressurepoint, std::vector<DM::Node*> &nodes)
{
	DM::Node* result = 0;
	nodes.push_back(currentpressurepoint);
	std::vector<DM::Component*> pipes = this->sys->getAllComponentsInView(wsd.getCompleteView(WS::PIPE,DM::READ));

	std::vector<DM::Edge*> edges = currentpressurepoint->getEdges();

	for(int index=0; index < edges.size(); index++)
	{
		DM::Edge* currentedge = edges[index];

		if(std::find(pipes.begin(),pipes.end(),currentedge)==pipes.end())
			continue;

		DM::Node* neighbournode = currentedge->getStartNode();

		if(neighbournode==currentpressurepoint)
			neighbournode = currentedge->getEndNode();

		if(std::find(nodes.begin(),nodes.end(),neighbournode)!=nodes.end())
			continue;

		double neighbourpressure = neighbournode->getAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure))->getDouble();

		if(neighbourpressure > 0)
			return neighbournode;

		neighbournode = getNearestPressure(neighbournode,nodes);

		if(!neighbournode)
			continue;

		neighbourpressure =  neighbournode->getAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure))->getDouble();

		if(neighbourpressure > 0)
			return neighbournode;
	}

	return result;
}

DM::Node* Dimensioning::getNearestFlowPoint(DM::Node* currentpoint, std::vector<DM::Node*> &nodes)
{
	DM::Node* result = 0;
	nodes.push_back(currentpoint);
	std::vector<DM::Component*> pipes = this->sys->getAllComponentsInView(wsd.getCompleteView(WS::PIPE,DM::READ));

	std::vector<DM::Edge*> edges = currentpoint->getEdges();

	for(int index=0; index < edges.size(); index++)
	{
		DM::Edge* currentedge = edges[index];

		if(std::find(pipes.begin(),pipes.end(),currentedge)==pipes.end())
			continue;

		DM::Node* neighbournode = currentedge->getStartNode();

		if(neighbournode==currentpoint)
			neighbournode = currentedge->getEndNode();

		if(std::find(nodes.begin(),nodes.end(),neighbournode)!=nodes.end())
			continue;

		if(getFlowNeighbours(neighbournode).size())
			return neighbournode;

		neighbournode = getNearestFlowPoint(neighbournode,nodes);

		if(!neighbournode)
			continue;

		return neighbournode;
	}

	return result;
}


bool Dimensioning::approximatePressure(bool discretediameter)
{
	int maxiteration = 2;
	std::vector<DM::Component*> reservoirs = this->sys->getAllComponentsInView(wsd.getCompleteView(WS::RESERVOIR,DM::READ));
	std::vector<DM::Component*> junctions = this->sys->getAllComponentsInView(wsd.getCompleteView(WS::JUNCTION,DM::READ));
	std::vector<DM::Component*> mainpipes = this->sys->getAllComponentsInView(wsd.getCompleteView(WS::MAINPIPE,DM::READ));
	std::vector<DM::Node*>knownpressure;
	std::vector<DM::Node*>unchecked;
	std::vector<DM::Node*>newpressurepoints;
	std::vector<DM::Node*>initunchecked;
	int iteration = 0;

	for(int index=0; index < junctions.size(); index++)
	{
		DM::Node* currentnode = dynamic_cast<DM::Node*>(junctions[index]);

		if(currentnode->getAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure))->getDouble()>0)
		{
			initunchecked.push_back(currentnode);

			if(!getFlowNeighbours(currentnode).size())
			{
				std::vector<DM::Node*> cn;
				double currentpressure = currentnode->getAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure))->getDouble();
				DM::Node* pressureneighbour = getNearestFlowPoint(currentnode,cn);

				if(pressureneighbour)
				{
					double pressure  = currentpressure - (pressureneighbour->getZ() - currentnode->getZ());
					pressureneighbour->changeAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure),pressure);
					initunchecked.push_back(pressureneighbour);
				}
			}
		}
	}

	for(int index=0; index < reservoirs.size(); index++)
		initunchecked.push_back(dynamic_cast<DM::Node*>(reservoirs[index]));

	/*
	if(discretediameter)
	{
		for(int index=0; index < mainpipes.size(); index++)
		{
			DM::Edge* currentedge = dynamic_cast<DM::Edge*>(mainpipes[index]);
			DM::Node* startnode = currentedge->getStartNode();
			DM::Node* endnode = currentedge->getEndNode();

			if(std::find(initunchecked.begin(),initunchecked.end(),startnode)==initunchecked.end())
				initunchecked.push_back(startnode);

			if(std::find(initunchecked.begin(),initunchecked.end(),endnode)==initunchecked.end())
				initunchecked.push_back(endnode);
		}
	}*/

	while((newpressurepoints.size()!=0 || !iteration) && iteration < maxiteration)
	{
		DM::Logger(DM::Standard) << "Iteration: " << iteration << " New pressure junctions: " << newpressurepoints.size();
		iteration++;
		for(int index=0; index < newpressurepoints.size(); index++)
			if(std::find(initunchecked.begin(),initunchecked.end(),newpressurepoints[index])==initunchecked.end())
				initunchecked.push_back(newpressurepoints[index]);

		for(int index=0; index < junctions.size(); index++)
		{
			DM::Node* currentnode = dynamic_cast<DM::Node*>(junctions[index]);

			if(currentnode->getAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure))->getDouble()>0)
				if(std::find(initunchecked.begin(),initunchecked.end(),currentnode)==initunchecked.end())
					currentnode->changeAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure),0);
		}

		knownpressure=initunchecked;
		newpressurepoints.clear();
		unchecked=knownpressure;

		if(!approximatePressure(knownpressure,unchecked,newpressurepoints,!(maxiteration > iteration)))
		{
			DM::Logger(DM::Error) << "Cannot aproximate Pressure";
			return false;
		}

		//approximate pressure for all leaf nodes
		for(int index=0; index < junctions.size(); index++)
		{
			DM::Node* currentnode = dynamic_cast<DM::Node*>(junctions[index]);

			if(currentnode->getAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure))->getDouble()<=0)
				if(getFlowNeighbours(currentnode).size()>0 && getInverseFlowNeighbours(currentnode).size()==0)
					unchecked.push_back(currentnode);
		}

		if(!approximatePressure(knownpressure,unchecked,newpressurepoints,!(maxiteration > iteration)))
			return false;

		if(newpressurepoints.size()>0)
			continue;

		//check pressure points with no flow
		for(int index=0; index < junctions.size(); index++)
		{
			DM::Node* currentnode = dynamic_cast<DM::Node*>(junctions[index]);

			if(currentnode->getAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure))->getDouble()==0.0)
			{
				std::vector<DM::Node*> kn;

				DM::Node* pressnode = getNearestPressure(currentnode,kn);

				double pressure = 0.0;

				if(pressnode)
				{
					pressure = pressnode->getAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure))->getDouble();
					pressure  = pressure + (pressnode->getZ() - currentnode->getZ());
				}

				currentnode->changeAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure),pressure);
				knownpressure.push_back(currentnode);
			}
		}

		DM::Logger(DM::Standard) << "Known pressurepoints: " << knownpressure.size() << " of " << junctions.size();
	}

	int notset=0;
	for(int index=0; index < junctions.size(); index++)
	{
		DM::Node* currentnode = dynamic_cast<DM::Node*>(junctions[index]);

		if(currentnode->getAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure))->getDouble()==0.0)
			notset++;
	}

	if(notset)
	{
		DM::Logger(DM::Error)<< "Pressure of " << notset << " junctions not set";
		//return false;
		return true;
	}

	return true;
}

bool Dimensioning::approximatePressure(std::vector<DM::Node*> &knownPressurePoints, std::vector<DM::Node*> &uncheckedpressurepoints,std::vector<DM::Node*> &newpressurepoint,bool nonewpressurepoints)
{
	if(uncheckedpressurepoints.size()==0)
		return true;

	DM::Node* currentpressurepoint = uncheckedpressurepoints[0];
	double currentpressure = currentpressurepoint->getAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure))->getDouble();
	std::vector<DM::Node*> neighbours = getFlowNeighbours(currentpressurepoint);

	for(int index=0; index<neighbours.size(); index++)
	{
		DM::Node* currentneighbour = neighbours[index];

		if(std::find(knownPressurePoints.begin(),knownPressurePoints.end(),currentneighbour)!=knownPressurePoints.end() && currentpressure > 0)
			continue;

		std::vector<DM::Node*> pathpoints;
		std::vector<DM::Node*> newuncheckedpressurepoints;

		if(!findFlowPath(pathpoints,newuncheckedpressurepoints,currentneighbour,knownPressurePoints))
			continue;

		pathpoints.insert(pathpoints.begin(),currentneighbour);
		pathpoints.insert(pathpoints.begin(),currentpressurepoint);

		if(!approximatePressureOnPath(pathpoints,knownPressurePoints,newpressurepoint,nonewpressurepoints))
			return false;

		for(int n=0; n<newuncheckedpressurepoints.size();n++)
			uncheckedpressurepoints.push_back(newuncheckedpressurepoints[n]);
	}

	uncheckedpressurepoints.erase(std::find(uncheckedpressurepoints.begin(),uncheckedpressurepoints.end(),currentpressurepoint));

	return approximatePressure(knownPressurePoints,uncheckedpressurepoints,newpressurepoint,nonewpressurepoints);
}

bool Dimensioning::approximatePressureOnPath(std::vector<DM::Node*> nodes,std::vector<DM::Node*> &knownPressurePoints,std::vector<DM::Node*> &newpressurepoints, bool nonewpressurepoints)
{
	//double apprdt = 0.0004;
	//double apprdt = 0.002;

	if(nodes.size() < 1)
		return false;

	//calculate totallengh
	double length = TBVectorData::calculateDistance(nodes);
	double startPressure = nodes[nodes.size()-1]->getAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure))->getDouble();
	double endPressure = nodes[0]->getAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure))->getDouble();

	double startZ = nodes[nodes.size()-1]->getZ();
	double endZ = nodes[0]->getZ();

	if(endPressure<=0 && getInverseFlowNeighbours(nodes[0]).size()==0)
	{
		endPressure = (startZ-endZ) - apprdt*length + startPressure;
		nodes[0]->changeAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure),endPressure);
		knownPressurePoints.push_back(nodes[0]);
	}

	double deltaP = startZ+startPressure-endZ-endPressure;
	double currentlength=length;

	if(deltaP < 0 && !nonewpressurepoints)
	{
		double forcedpressure = (startZ-endZ) + apprdt*length + startPressure;
		nodes[0]->changeAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure),forcedpressure);
		newpressurepoints.push_back(nodes[0]);
		return true;
	}

	if(deltaP < 0)
		DM::Logger(DM::Warning) << "Found flow path with negative headloss -> PUMP?";

	for(int index=1; index<nodes.size()-1;index++)
	{
		currentlength-=TBVectorData::calculateDistance(nodes[index-1],nodes[index]);
		double newPressure = startZ+startPressure - nodes[index]->getZ() - (deltaP/length)*currentlength;

		//if(newPressure<=0)
		//	DM::Logger(DM::Warning) << "Negative pressure approximated: " << newPressure << " m";

		nodes[index]->changeAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure),newPressure);
		knownPressurePoints.push_back(nodes[index]);
	}

	return true;
}

bool Dimensioning::findFlowPath(std::vector<DM::Node*> &nodes, std::vector<DM::Node*> &alternativepathjunction, DM::Node* currentPressurePoint, std::vector<DM::Node*> knownPressurePoints)
{
	std::vector<DM::Node*> neighbours = getFlowNeighbours(currentPressurePoint);

	while(neighbours.size() > 0)
	{
		if(std::find(knownPressurePoints.begin(),knownPressurePoints.end(),currentPressurePoint)!=knownPressurePoints.end())
			break;

		if(neighbours.size()>1)
			alternativepathjunction.push_back(currentPressurePoint);

		DM::Node* next = neighbours[0];

		//cycle check
		if(std::find(nodes.begin(),nodes.end(),next)!=nodes.end())
		{
			DM::Logger(DM::Error) << "Found cyclic flow path";
			nodes.clear();
			return true;
		}

		nodes.push_back(next);
		neighbours = getFlowNeighbours(next);
		currentPressurePoint=next;
	}

	if(std::find(knownPressurePoints.begin(),knownPressurePoints.end(),currentPressurePoint)==knownPressurePoints.end())
	{
		DM::Logger(DM::Error) << "Pressure of starting point in flow path is not known: " << currentPressurePoint->getAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure))->getDouble();
		return false;
	}

	return true;
}

bool Dimensioning::approximatePipeSizes(bool usemainpipes,bool discretediameter)
{
	std::vector<DM::Component*> allpipes = this->sys->getAllComponentsInView(wsd.getCompleteView(WS::PIPE,DM::READ));
	std::vector<DM::Component*> mainpipes = this->sys->getAllComponentsInView(wsd.getCompleteView(WS::MAINPIPE,DM::READ));

	for(int index=0; index<allpipes.size(); index++)
	{
		DM::Edge* currentpipe = dynamic_cast<DM::Edge*>(allpipes[index]);
		DM::Node* startnode = currentpipe->getStartNode();
		DM::Node* endnode = currentpipe->getEndNode();
		uint epanetID = converter->getEpanetLinkID(currentpipe);
		float flow = 0;
		if(!converter->checkENRet(EPANET::ENgetlinkvalue(epanetID,EN_FLOW,&flow)))return false;

		double startZ=startnode->getZ();
		double endZ=endnode->getZ();
		double startP=startnode->getAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure))->getDouble();
		double endP=endnode->getAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure))->getDouble();

		if(std::fabs(flow) < 0.001)
			flow=0.0;

		if(flow<0)
		{
			flow=-flow;
			endZ=startnode->getZ();
			startZ=endnode->getZ();
			endP=startnode->getAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure))->getDouble();
			startP=endnode->getAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure))->getDouble();
		}

		double pressureDiff = startZ+startP-endZ-endP;

		if(std::fabs(pressureDiff) < 0.0000001)
			pressureDiff=0.0;

		double diameter = 0.0;
		double roughness = 0.004; //roughness (m)

		if(usemainpipes && std::find(mainpipes.begin(),mainpipes.end(),currentpipe)!=mainpipes.end())
			roughness = 0.001;

		if(pressureDiff < 0 && flow > 0)
		{
			if(!(startP > 0 && endP > 0))
			{
				DM::Logger(DM::Warning) << "Pipe with negative pressure difference: " << pressureDiff << " Flow: " << flow;
				DM::Logger(DM::Warning) << "Please place a pump here or use more observed pressure points in your system to balance all reservoirs";
				diameter = calcDiameter(roughness,TBVectorData::calculateDistance(currentpipe->getStartNode(),currentpipe->getEndNode()),flow/1000.0,std::fabs(pressureDiff),maxdiameter,discretediameter);
			}
			else
			{
				diameter = calcDiameter(roughness,TBVectorData::calculateDistance(currentpipe->getStartNode(),currentpipe->getEndNode()),flow/1000.0,std::fabs(pressureDiff),maxdiameter,discretediameter);
			}
		}
		else
		{
			diameter = calcDiameter(roughness,TBVectorData::calculateDistance(currentpipe->getStartNode(),currentpipe->getEndNode()),flow/1000.0,pressureDiff,maxdiameter,discretediameter);
		}

		if(!converter->checkENRet(EPANET::ENsetlinkvalue(epanetID,EN_DIAMETER,diameter*1000)))return false;
		if(!converter->checkENRet(EPANET::ENsetlinkvalue(epanetID,EN_ROUGHNESS,roughness*1000)))return false;
		currentpipe->changeAttribute(wsd.getAttributeString(DM::WS::PIPE,DM::WS::PIPE_ATTR_DEF::Diameter),diameter);
		currentpipe->changeAttribute(wsd.getAttributeString(DM::WS::PIPE,DM::WS::PIPE_ATTR_DEF::Roughness),roughness*1000);
	}

	if(!converter->checkENRet(EPANET::ENsolveH()))return false;

	return true;
}

bool Dimensioning::approximateMainPipes(bool usereservoirsdata, double totaldemand, std::vector<DM::Edge*> &respipes,bool discretediameters)
{
	double roughness = 0.0001; //roughness (m)
	std::vector<DM::Component*> reservoirs = this->sys->getAllComponentsInView(wsd.getCompleteView(WS::RESERVOIR,DM::READ));
	std::vector<DM::Component*> mainpipe = this->sys->getAllComponentsInView(wsd.getCompleteView(WS::MAINPIPE,DM::READ));
	std::map<DM::Component*, DM::Edge*> entrypipes;

	std::vector<double> supplypercent;
	double defaultdiameter = calcDiameter(roughness,1000,totaldemand*0.75,1,maxdiameter/1000.0,discretediameters) * 1000;

	for(int index=0; index<reservoirs.size(); index++)
		if(usereservoirsdata)
			supplypercent.push_back(reservoirs[index]->getAttribute(wsd.getAttributeString(DM::WS::RESERVOIR,DM::WS::RESERVOIR_ATTR_DEF::SupplyPercent))->getDouble()/100.0);
		else
			supplypercent.push_back(1.0/reservoirs.size());

	for(int index = 0; index < mainpipe.size(); index++)
	{
		DM::Edge* currentedge = dynamic_cast<DM::Edge*>(mainpipe[index]);

		if(std::find(reservoirs.end(),reservoirs.begin(),currentedge->getStartNode())!=reservoirs.end())
			entrypipes[currentedge->getStartNode()] = currentedge;

		if(std::find(reservoirs.end(),reservoirs.begin(),currentedge->getEndNode())!=reservoirs.end())
			entrypipes[currentedge->getEndNode()] = currentedge;

		currentedge->changeAttribute(wsd.getAttributeString(DM::WS::PIPE,DM::WS::PIPE_ATTR_DEF::Diameter),defaultdiameter);
		currentedge->changeAttribute(wsd.getAttributeString(DM::WS::PIPE,DM::WS::PIPE_ATTR_DEF::Roughness),roughness*1000);
	}

	if(reservoirs.size() > entrypipes.size())
		return false;


	for(int index=0; index < reservoirs.size(); index++)
	{
		DM::Edge* currentedge = entrypipes[reservoirs[index]];
		respipes.push_back(currentedge);

		if(usereservoirsdata)
			currentedge->changeAttribute(wsd.getAttributeString(DM::WS::PIPE,DM::WS::PIPE_ATTR_DEF::Flow),totaldemand*supplypercent[index]*1000);
	}

	return true;
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

bool Dimensioning::calibrateReservoirOutFlow(double totaldemand, int maxsteps, std::vector<DM::Edge*> entrypipes, bool discretediameter)
{
	if(discretediameter)
		DM::Logger(DM::Error) << "Discrete diameters are not implemented during calibrating reservoir outflow";

	double diameterstepsize = 0.01;
	for(int index=0; index < entrypipes.size(); index++)
	{
		DM::Edge* currentpipe = entrypipes[index];
		double assumedflow = currentpipe->getAttribute(wsd.getAttributeString(DM::WS::PIPE,DM::WS::PIPE_ATTR_DEF::Flow))->getDouble();
		float diameter = std::sqrt(4*assumedflow/1000.0)/M_PI*1000;
		currentpipe->changeAttribute(wsd.getAttributeString(DM::WS::PIPE,DM::WS::PIPE_ATTR_DEF::Diameter),diameter);
		uint epanetID = converter->getEpanetLinkID(currentpipe);
		if(!converter->checkENRet(EPANET::ENsetlinkvalue(epanetID,EN_DIAMETER,diameter)))return false;
	}

	if(!converter->checkENRet(EPANET::ENsolveH()))return false;

	for(int index=0; index<maxsteps; index++)
	{
		double totalerror = 0.0;
		bool ok = true;
		for(int p = 0; p < entrypipes.size(); p++)
		{
			DM::Edge* currentpipe=entrypipes[p];
			uint epanetID = converter->getEpanetLinkID(currentpipe);
			double assumedflow = currentpipe->getAttribute(wsd.getAttributeString(DM::WS::PIPE,DM::WS::PIPE_ATTR_DEF::Flow))->getDouble();
			float flow;
			float diameter;

			if(!converter->checkENRet(EPANET::ENgetlinkvalue(epanetID,EN_FLOW,&flow)))return false;
			if(!converter->checkENRet(EPANET::ENgetlinkvalue(epanetID,EN_DIAMETER,&diameter)))return false;

			//DM::Logger(DM::Standard) << "DIFF: " << (assumedflow -flow);

			if(std::fabs(flow-assumedflow)/assumedflow > 0.10)
				ok = false;

			totalerror += std::fabs(flow-assumedflow);

			if(flow < assumedflow)
				diameter+=(diameterstepsize*1000.0);
			else
				diameter-=(diameterstepsize*1000.0);

			if(diameter <= 0.0)
				diameter = 0.0001;

			if(!converter->checkENRet(EPANET::ENsetlinkvalue(epanetID,EN_DIAMETER,diameter)))return false;
			currentpipe->changeAttribute(wsd.getAttributeString(DM::WS::PIPE,DM::WS::PIPE_ATTR_DEF::Diameter),diameter);
			EPANET::ENsolveH();

		}

		diameterstepsize = totalerror/totaldemand/10000.0;

		if( ok )
			break;
	}

	return true;
}

double Dimensioning::calcDiameter(double k, double l, double q, double h, double maxdiameter, bool discretediameters)
{
	std::vector<double> diameters;
	diameters.push_back(0.080);
	diameters.push_back(0.100);
	diameters.push_back(0.125);
	diameters.push_back(0.150);
	diameters.push_back(0.200);
	diameters.push_back(0.250);
	diameters.push_back(0.300);
	diameters.push_back(0.350);
	diameters.push_back(0.400);
	diameters.push_back(0.500);
	diameters.push_back(0.600);
	diameters.push_back(0.800);
	diameters.push_back(1.000);
	diameters.push_back(1.500);
	diameters.push_back(2.000);
	diameters.push_back(4.000);
	diameters.push_back(8.000);

	if(q < 0.00001)
		return 0.01;

	if(h < 0)
		return -1;

	if(h/l < 0.00001)
	{
		return std::sqrt(4*q)/M_PI;
	}

	double maxerror = 0.0000001;
	double d = 0.001;
	double maxd = 6;
	double mind = 0;
	double currenterror = h-calcFrictionHeadLoss(d,k,l,q);

	while (std::fabs(currenterror) > maxerror)
	{
		//DM::Logger(DM::Error) << currenterror << " " << maxerror;
		if(currenterror < 0)
		{
			mind=d;
			d = d+(maxd-mind)/2;
		}

		if(currenterror > 0)
		{
			maxd=d;
			d = d-(maxd-mind)/2;
		}

		double frictionhl = calcFrictionHeadLoss(d,k,l,q);
		double olderror = currenterror;
		currenterror = h-frictionhl;

		if(olderror==currenterror)
			break;

		DM::Logger(DM::Debug) << "Friction: " << frictionhl;
	}

	if(d > maxdiameter)
		d =  maxdiameter;

	if(discretediameters)
	{
		for (int i = 0; i < diameters.size(); ++i)
			if(d <= diameters[i])
			{
				if(nearestdiscretediameter)
					if(i!=0)
					{
						if((d-diameters[i-1]) < (diameters[i]-d))
							i--;
					}

				d=diameters[i];
				break;
			}
	}

	if(d < diameters[0])
		d = diameters[0];

	return d;
}

double Dimensioning::calcFrictionHeadLoss(double d, double k, double l, double q)
{
	double headloss = 0;

	headloss =
			calcLambda(k,d,q)*
			(l/d)*
			(std::pow(q/((std::pow(d,2)/4.0)*M_PI),2)/(2*9.81));

	return headloss;
}

double Dimensioning::calcLambda(double k, double d, double q, double lambda)
{
	double maxerror = 0.01;

	if(lambda==0)
	{
		lambda = 1.0/std::pow(2*std::log10((3.71*d)/k),2);
		return calcLambda(k,d,q,lambda);
	}
	else
	{
		double viscosity = 1.3*std::pow((double)10,-6);
		double v = q/((std::pow(d,2)/4.0)*M_PI);
		double Re = (v*d)/viscosity;
		double tmpl = -2*std::log10((2.51/(Re*std::sqrt(lambda)))+(k/(3.71*d)));
		double error = std::fabs(1/std::sqrt(lambda) - tmpl);
		double oldlambda = lambda;
		lambda = 1/std::pow(-tmpl,2);

		if(error > maxerror && oldlambda != lambda)
		{
			return calcLambda(k,d,q,lambda);
		}
		else
		{
			return lambda;
		}
	}

	return lambda;
}
