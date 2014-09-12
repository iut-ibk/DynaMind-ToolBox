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
#include <QUuid>

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
	this->savesubsurface=false;
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
	this->addParameter("Save approximated total head", DM::BOOL, &this->savesubsurface);
}

void Dimensioning::init()
{
	std::vector<DM::View> views;

	DM::View jview = wsd.getCompleteView(WS::JUNCTION,DM::MODIFY);

	if(savesubsurface)
		jview.addAttribute("Approximated_Head",DM::Attribute::DOUBLE,DM::WRITE);

	views.push_back(jview);
	views.push_back(wsd.getCompleteView(WS::PIPE,DM::MODIFY));
	views.push_back(wsd.getCompleteView(WS::RESERVOIR,DM::READ));

	if(usemainpipe)
		views.push_back(wsd.getCompleteView(WS::MAINPIPE,DM::MODIFY));

	this->addData("Watersupply", views);
}

void Dimensioning::run()
{
	QString dir = QDir::tempPath();

	std::string name = QUuid::createUuid().toString().mid(1,36).toStdString();

	#if defined DEBUG || _DEBUG
	name = "test";
	#endif

	std::string inpfilename = dir.toStdString() + "/" + name +".inp";
	std::string rptfilename = dir.toStdString() + "/" + name +".rpt";

	if(usemainpipe)
		fixeddiameters=true;

	converter.reset();
	converter = boost::make_shared<EpanetDynamindConverter>();
	DM::Logger(DM::Standard) << "Epanet loaded";

	this->sys = this->getData("Watersupply");

	if(fixeddiameters)
		searchFixedPipes(80,fixedpipes,this->sys->getAllComponentsInView(wsd.getCompleteView(WS::PIPE,DM::READ)));

	double totaldemand = calcTotalDemand();
	std::vector<DM::Edge*> entrypipes;

	if(usemainpipe)
	{
		if(!approximateMainPipes(usereservoirdata,totaldemand,entrypipes,discrete))return;
		DM::Logger(DM::Standard) << "Approximate main pipes .... DONE";
	}

	DM::Logger(DM::Standard) << "Writing file: " << inpfilename;
	if(!converter->createEpanetModel(this->sys,inpfilename))
	{
		DM::Logger(DM::Error) << "Could not create a valid EPANET inp file";
		return;
	}

	DM::Logger(DM::Standard) << "Open Epanet model";
	if(!converter->openEpanetModel(inpfilename,rptfilename)) return;

	//set small simulation duration and timesteps
	EPANET::ENsettimeparam(EN_DURATION,60);
	EPANET::ENsettimeparam(EN_HYDSTEP,30);

	DM::Logger(DM::Standard) << "Start pipe sizing";

	if(!SitzenfreiDimensioning())return;

	//if(usemainpipe)
	{
		DM::Logger(DM::Standard) << "Using main pipe information";
		initknownpressure = getInitialPressurepoints();

		for (int var = 0; var < this->iterations; ++var)
		{
			if(usereservoirdata && usemainpipe)
			{
				if(!calibrateReservoirOutFlow(totaldemand, 100,entrypipes,false))
					return;

				DM::Logger(DM::Standard) << "Calibrate reservoir out flow .... DONE";
			}
			else
			{
				EPANET::ENsolveH();
			}

			DM::Logger(DM::Standard) << "Approximate pressure";

			if(!approximatePressure(discrete,initknownpressure))
				return;

			DM::Logger(DM::Standard) << "Approximate pressure .... DONE";

			if(!approximatePipeSizes(true,discrete))
				return;

			DM::Logger(DM::Standard) << "Approximate pipe sizes .... DONE";

			//resetPressureOfJunctions(initknownpressure);
		}

		if(discrete)
			if(!approximatePipeSizes(true,discrete))
				return;
	}

	if(savesubsurface)
	{
		std::vector<DM::Component*> junctions = this->sys->getAllComponentsInView(wsd.getCompleteView(WS::JUNCTION,DM::MODIFY));

		for(int index=0; index<junctions.size(); index++)
		{
			DM::Node *j = static_cast<DM::Node*>(junctions[index]);
			double p = j->getAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure))->getDouble();
			double h = j->getZ() + p;
			j->changeAttribute("Approximated_Head",h);
		}
	}

	if(!converter->mapEpanetAttributes(this->sys)) return;

	converter->closeEpanetModel();

	DM::Logger(DM::Standard) << "Total demand: " << totaldemand << " m^3/s";

	QFile::remove(QString::fromStdString(inpfilename));
	QFile::remove(QString::fromStdString(rptfilename));
	converter.reset();
}

double Dimensioning::calcTotalDemand()
{
	double sum = 0;
	std::vector<DM::Component*> junctions = this->sys->getAllComponentsInView(wsd.getCompleteView(WS::JUNCTION,DM::READ));

	for(int index=0; index<junctions.size(); index++)
		sum += junctions[index]->getAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Demand))->getDouble();

	return sum/1000.0;
}

void Dimensioning::searchFixedPipes(double maxdiameter, std::vector<Component *> &fixedpipes, std::vector<Component *> allpipes)
{
	for (int i = 0; i < allpipes.size(); ++i)
		if(allpipes[i]->getAttribute(wsd.getAttributeString(DM::WS::PIPE,DM::WS::PIPE_ATTR_DEF::Diameter))->getDouble() >= maxdiameter)
			fixedpipes.push_back(allpipes[i]);
}

bool Dimensioning::approximatePressure(bool discretediameter,std::vector<DM::Node*>initunchecked)
{
	std::vector<DM::Component*> reservoirs = this->sys->getAllComponentsInView(wsd.getCompleteView(WS::RESERVOIR,DM::READ));
	std::vector<DM::Component*> junctions = this->sys->getAllComponentsInView(wsd.getCompleteView(WS::JUNCTION,DM::READ));
	//std::vector<DM::Component*> mainpipes = this->sys->getAllComponentsInView(wsd.getCompleteView(WS::MAINPIPE,DM::READ));

	std::vector<DM::Node*>knownpressure;
	std::vector<DM::Node*>unchecked;
	std::vector<DM::Node*>newinitunchecked;

	//Balance all loops or systems with more than one water source
	//Do not change max iteration --- it seams we to not have to iterate to get good results
	for (int iteration = 0; iteration < 30; iteration++)
	{
		if(newinitunchecked.size()==0 && iteration>0)
			break;

		knownpressure = initunchecked;
		knownpressure.insert(knownpressure.end(), newinitunchecked.begin(), newinitunchecked.end() );
		DM::Logger(DM::Standard) << "Approximate pressure internal iteration number " << iteration;
		DM::Logger(DM::Standard) << "Known pressurepoints at begining of approximation: " << knownpressure.size() << " of " << junctions.size() << " [ " << newinitunchecked.size() << "new initial points]";
		newinitunchecked.clear();

		for(int index=0; index < junctions.size(); index++)
		{
			DM::Node* currentnode = dynamic_cast<DM::Node*>(junctions[index]);

			if(currentnode->getAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure))->getDouble()!=0.0)
				if(std::find(knownpressure.begin(),knownpressure.end(),currentnode)==knownpressure.end())
					currentnode->changeAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure),0);
		}

		unchecked=knownpressure;

		DM::Logger(DM::Standard) << "Approximate pressure for all non leaf nodes";

		if(!approximatePressure(knownpressure,unchecked,newinitunchecked))
		{
			DM::Logger(DM::Error) << "Cannot approximate Pressure";
			return false;
		}

		DM::Logger(DM::Standard) << "Known pressurepoints after checking all non leaf nodes: " << knownpressure.size() << " of " << junctions.size();
		DM::Logger(DM::Standard) << "Searching all leaf nodes";

		//approximate pressure for all leaf nodes
		for(int index=0; index < junctions.size(); index++)
		{
			DM::Node* currentnode = dynamic_cast<DM::Node*>(junctions[index]);

			if(currentnode->getAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure))->getDouble()==0.0)
				if(converter->getFlowNeighbours(currentnode).size()>0 && converter->getInverseFlowNeighbours(currentnode).size()==0)
					if(std::find(knownpressure.begin(),knownpressure.end(),currentnode)==knownpressure.end())
						unchecked.push_back(currentnode);
		}

		DM::Logger(DM::Standard) << "Approximate pressure for all leaf nodes";

		if(!approximatePressure(knownpressure,unchecked,newinitunchecked))
		{
			DM::Logger(DM::Error) << "Cannot approximate Pressure";
			return false;
		}

		DM::Logger(DM::Standard) << "Known pressurepoints after checking all leaf nodes: " << knownpressure.size() << " of " << junctions.size();

		DM::Logger(DM::Standard) << "Check pressure points with no flow";

		//check pressure points with no flow
		for(int index=0; index < junctions.size(); index++)
		{
			DM::Node* currentnode = dynamic_cast<DM::Node*>(junctions[index]);

			if(currentnode->getAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure))->getDouble()==0.0)
			{
				if(std::find(knownpressure.begin(), knownpressure.end(),currentnode)!=knownpressure.end())
					continue;

				std::vector<DM::Node*> kn;

				DM::Node* pressnode = converter->getNearestPressure(currentnode,kn);

				double pressure = 0.0;

				if(pressnode)
				{
					pressure = pressnode->getAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure))->getDouble();
					pressure  = pressure + (pressnode->getZ() - currentnode->getZ());
				}

				currentnode->changeAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure),pressure);

				if(std::find(knownpressure.begin(), knownpressure.end(),currentnode)==knownpressure.end())
					knownpressure.push_back(currentnode);
			}
		}

		DM::Logger(DM::Standard) << "Known pressurepoints after checking points with no flow: " << knownpressure.size() << " of " << junctions.size();
		DM::Logger(DM::Standard) << "Check pressure junctions which are still not approximated";

		for(int index=0; index < knownpressure.size(); index++)
		{
			DM::Node* currentnode = dynamic_cast<DM::Node*>(knownpressure[index]);

			if(!currentnode)
				continue;

			if(std::find(junctions.begin(),junctions.end(),currentnode)==junctions.end())
				unchecked.push_back(currentnode);
		}

		if(!approximatePressure(knownpressure,unchecked,newinitunchecked))
			return false;

		DM::Logger(DM::Standard) << "Known pressurepoints after checking all junctions: " << knownpressure.size() << " of " << junctions.size();
	}

	return true;
}

bool Dimensioning::approximatePressure(std::vector<DM::Node*> &knownPressurePoints, std::vector<DM::Node*> &uncheckedpressurepoints, std::vector<DM::Node*> &newinitunchecked)
{
	std::vector<DM::Node*> checkedpressurepoints;

	if(uncheckedpressurepoints.size()==0)
		return true;

	while(uncheckedpressurepoints.size() > 0)
	{
		DM::Node* currentpressurepoint = uncheckedpressurepoints[0];

		double currentpressure = currentpressurepoint->getAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure))->getDouble();
		std::vector<DM::Node*> neighbours = converter->getFlowNeighbours(currentpressurepoint);

		for(int index=0; index<neighbours.size(); index++)
		{
			DM::Node* currentneighbour = neighbours[index];

			if(std::find(knownPressurePoints.begin(),knownPressurePoints.end(),currentneighbour)!=knownPressurePoints.end() && currentpressure > 0)
				continue;

			std::vector<DM::Node*> pathpoints;
			std::vector<DM::Node*> newuncheckedpressurepoints;

			if(!findFlowPath(pathpoints,newuncheckedpressurepoints,currentneighbour,knownPressurePoints))
				continue;

			if(std::find(pathpoints.begin(),pathpoints.end(),currentneighbour)==pathpoints.end())
				pathpoints.insert(pathpoints.begin(),currentneighbour);

			pathpoints.insert(pathpoints.begin(),currentpressurepoint);

			if(!approximatePressureOnPath(pathpoints,knownPressurePoints, newinitunchecked))
				return false;

			for(int n=0; n<newuncheckedpressurepoints.size();n++)
				if(std::find(checkedpressurepoints.begin(),checkedpressurepoints.end(),newuncheckedpressurepoints[n])==checkedpressurepoints.end())
					if(std::find(knownPressurePoints.begin(),knownPressurePoints.end(),newuncheckedpressurepoints[n])==knownPressurePoints.end())
					{
						if(std::find(uncheckedpressurepoints.begin(),uncheckedpressurepoints.end(),newuncheckedpressurepoints[n])!=uncheckedpressurepoints.end())
							uncheckedpressurepoints.erase(std::find(uncheckedpressurepoints.begin(),uncheckedpressurepoints.end(),newuncheckedpressurepoints[n]));

						uncheckedpressurepoints.insert(uncheckedpressurepoints.begin(),sizeof(newuncheckedpressurepoints[n]),newuncheckedpressurepoints[n]);
					}
		}

		checkedpressurepoints.push_back(currentpressurepoint);
		uncheckedpressurepoints.erase(std::find(uncheckedpressurepoints.begin(),uncheckedpressurepoints.end(),currentpressurepoint));
	}
	return true;
}

bool Dimensioning::approximatePressureOnPath(std::vector<DM::Node*> nodes,std::vector<DM::Node*> &knownPressurePoints, std::vector<DM::Node*> &newinitunchecked)
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

	//set pressure for leaf nodes
	if(endPressure == 0.0 && converter->getInverseFlowNeighbours(nodes[0]).size()==0)
	{
		endPressure = (startZ-endZ) - apprdt*length + startPressure;
		nodes[0]->changeAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure),endPressure);
	}

	double deltaP = startZ+startPressure-endZ-endPressure;
	double currentlength=length;

	/*
	if(deltaP < 0)
	{
		//Wrong flowpath for the current pressure surface
		if(std::find(knownPressurePoints.begin(),knownPressurePoints.end(),nodes[0])==knownPressurePoints.end())
		{
			bool alreadyforced=false;

			if(std::find(newinitunchecked.begin(),newinitunchecked.end(),nodes[0])==newinitunchecked.end())
				newinitunchecked.push_back(nodes[0]);
			else
				alreadyforced= true;


			double forcedpressure = startPressure + startZ + apprdt*length - endZ;

			if(alreadyforced)
				forcedpressure=std::max(forcedpressure,endPressure);

			nodes[0]->changeAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure),forcedpressure);
			endPressure = forcedpressure;
			deltaP = startZ+startPressure-endZ-endPressure;
		}
	}
	*/

	if(std::find(knownPressurePoints.begin(),knownPressurePoints.end(),nodes[0])==knownPressurePoints.end())
		knownPressurePoints.push_back(nodes[0]);

	for(int index=1; index<nodes.size()-1;index++)
	{
		currentlength-=TBVectorData::calculateDistance(nodes[index-1],nodes[index]);
		double newPressure = startZ+startPressure - nodes[index]->getZ() - (deltaP/length)*currentlength;

		nodes[index]->changeAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure),newPressure);
		if(std::find(knownPressurePoints.begin(),knownPressurePoints.end(),nodes[index])==knownPressurePoints.end())
			knownPressurePoints.push_back(nodes[index]);
	}

	return true;
}

std::vector<Node *> Dimensioning::getInitialPressurepoints()
{
	std::vector<DM::Component*> reservoirs = this->sys->getAllComponentsInView(wsd.getCompleteView(WS::RESERVOIR,DM::READ));
	std::vector<DM::Component*> junctions = this->sys->getAllComponentsInView(wsd.getCompleteView(WS::JUNCTION,DM::READ));
	//std::vector<DM::Component*> mainpipes = this->sys->getAllComponentsInView(wsd.getCompleteView(WS::MAINPIPE,DM::READ));

	std::vector<DM::Node*>initunchecked;
	int noflowpressurepoints=0;

	for(int index=0; index < junctions.size(); index++)
	{
		DM::Node* currentnode = dynamic_cast<DM::Node*>(junctions[index]);

		if(currentnode->getAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure))->getDouble()>0)
		{
			if(std::find(initunchecked.begin(),initunchecked.end(),currentnode)==initunchecked.end())
				initunchecked.push_back(currentnode);

			if(!converter->getFlowNeighbours(currentnode).size())
			{
				std::vector<DM::Node*> cn;
				double currentpressure = currentnode->getAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure))->getDouble();
				DM::Node* pressureneighbour = converter->getNearestFlowPoint(currentnode,cn);

				if(pressureneighbour)
				{
					double pressure  = currentpressure - (pressureneighbour->getZ() - currentnode->getZ());
					pressureneighbour->changeAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure),pressure);

					if(std::find(initunchecked.begin(),initunchecked.end(),pressureneighbour)==initunchecked.end())
					{
						initunchecked.push_back(pressureneighbour);
						noflowpressurepoints++;
					}
				}
			}
		}
	}

	DM::Logger(DM::Standard) << "Given pressure points: " << initunchecked.size() - noflowpressurepoints << " + " << noflowpressurepoints << " points with no flow";

	for(int index=0; index < reservoirs.size(); index++)
		initunchecked.push_back(dynamic_cast<DM::Node*>(reservoirs[index]));

	DM::Logger(DM::Standard) << "Given pressure points + reservoirs: " << initunchecked.size();

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

	return initunchecked;
}

void Dimensioning::resetPressureOfJunctions(std::vector<Node *> nodexceptions)
{
	std::vector<DM::Component*> junctions = this->sys->getAllComponentsInView(wsd.getCompleteView(WS::JUNCTION,DM::READ));

	for (int index = 0; index < junctions.size(); ++index)
	{
		DM::Component *currentnode = junctions[index];
		if(std::find(nodexceptions.begin(),nodexceptions.end(),currentnode)==nodexceptions.end())
			currentnode->changeAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure),0.0);
	}
}

bool Dimensioning::findFlowPath(std::vector<DM::Node*> &nodes, std::vector<DM::Node*> &alternativepathjunction, DM::Node* currentPressurePoint, std::vector<DM::Node*> knownPressurePoints)
{
	std::vector<DM::Node*> neighbours = converter->getFlowNeighbours(currentPressurePoint);

	while(neighbours.size() > 0)
	{
		if(std::find(knownPressurePoints.begin(),knownPressurePoints.end(),currentPressurePoint)!=knownPressurePoints.end())
			break;

		if(neighbours.size()>1)
			if(std::find(alternativepathjunction.begin(),alternativepathjunction.end(),currentPressurePoint)==alternativepathjunction.end())
				alternativepathjunction.push_back(currentPressurePoint);

		DM::Node* next = neighbours[0];

		//cycle check -- This should not happen but epanet has some minor cyclic flow -> maybe a treshold sould be implemented
		if(std::find(nodes.begin(),nodes.end(),next)!=nodes.end())
		{
			DM::Logger(DM::Debug) << "Found cyclic flow path of length "  << nodes.size();
			nodes.clear();
			return true;
		}

		nodes.push_back(next);
		currentPressurePoint=next;
		neighbours = converter->getFlowNeighbours(currentPressurePoint);
	}

	if(std::find(knownPressurePoints.begin(),knownPressurePoints.end(),currentPressurePoint)==knownPressurePoints.end())
		return false;

	return true;
}

double Dimensioning::findShortestFlowPath(std::vector<DM::Node*> &nodes, std::vector<DM::Node*> &alternativepathjunction, DM::Node* currentPressurePoint, std::vector<DM::Node*> knownPressurePoints)
{
	//Only a alternative version of creating a pressuresubsurface (currently not working correct and its slow)
	std::vector<DM::Node*> neighbours = converter->getFlowNeighbours(currentPressurePoint);

	while(neighbours.size() == 1)
	{
		if(std::find(knownPressurePoints.begin(),knownPressurePoints.end(),currentPressurePoint)!=knownPressurePoints.end())
			return TBVectorData::calculateDistance(nodes);

		DM::Node* next = neighbours[0];

		//cycle check -- This should not happen but epanet has some minor cyclic flow -> maybe a treshold sould be implemented
		if(std::find(nodes.begin(),nodes.end(),next)!=nodes.end())
		{
			DM::Logger(DM::Error) << "Found cyclic flow path of length "  << nodes.size();
			nodes.clear();
			return 100000000000000000;
		}

		nodes.push_back(next);
		currentPressurePoint=next;
		neighbours = converter->getFlowNeighbours(currentPressurePoint);
	}

	if(neighbours.size() > 1)
	{
		if(std::find(nodes.begin(),nodes.end(),currentPressurePoint)==nodes.end())
			nodes.push_back(currentPressurePoint);

		alternativepathjunction.push_back(currentPressurePoint);

		double shortestdist = 0;
		std::vector<DM::Node*> shortestalternativepathjunctions;
		std::vector<DM::Node*> shortestNodes;

		for (int n = 0; n < neighbours.size(); ++n)
		{
			std::vector<DM::Node*> sapj;
			std::vector<DM::Node*> sn;
			double sd = findShortestFlowPath(sn,sapj,neighbours[n],knownPressurePoints);

			if(n==0 || sd < shortestdist)
			{
				shortestdist = sd;
				shortestalternativepathjunctions = sapj;
				shortestNodes = sn;
			}
		}

		nodes.insert(nodes.end(),shortestNodes.begin(),shortestNodes.end());
		alternativepathjunction.insert(alternativepathjunction.end(),shortestalternativepathjunctions.begin(),shortestalternativepathjunctions.end());
	}
	else
	{
		nodes.push_back(currentPressurePoint);
	}

	return TBVectorData::calculateDistance(nodes);
}

bool Dimensioning::approximatePipeSizes(bool usemainpipes,bool discretediameter)
{
	double minmainpipediameter = 0.2; //(m)

	std::vector<DM::Component*> allpipes = this->sys->getAllComponentsInView(wsd.getCompleteView(WS::PIPE,DM::READ));
	std::vector<DM::Component*> mainpipes = this->sys->getAllComponentsInView(wsd.getCompleteView(WS::MAINPIPE,DM::READ));
	std::vector<DM::Component*> reservoirs = this->sys->getAllComponentsInView(wsd.getCompleteView(WS::RESERVOIR,DM::READ));

	for(int index=0; index<allpipes.size(); index++)
	{
		DM::Edge* currentpipe = dynamic_cast<DM::Edge*>(allpipes[index]);

		if(std::find(fixedpipes.begin(),fixedpipes.end(),currentpipe)!=fixedpipes.end())
			continue;

		DM::Node* startnode = currentpipe->getStartNode();
		DM::Node* endnode = currentpipe->getEndNode();
		uint epanetID = converter->getEpanetLinkID(currentpipe);
		float flow = 0;
		if(!converter->checkENRet(EPANET::ENgetlinkvalue(epanetID,EN_FLOW,&flow)))return false;

		double startZ=startnode->getZ();
		double endZ=endnode->getZ();
		double startP=startnode->getAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure))->getDouble();
		double endP=endnode->getAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure))->getDouble();
		double length = currentpipe->getAttribute(wsd.getAttributeString(DM::WS::PIPE,DM::WS::PIPE_ATTR_DEF::Length))->getDouble();

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
		double roughness = 0.0004; //roughness (m)

		if(usemainpipes && std::find(mainpipes.begin(),mainpipes.end(),currentpipe)!=mainpipes.end())
			roughness = 0.0001;

		/*
		if(startP > 0 && endP < 0)
		{
			//TODO implement Pumpts here
			DM::Logger(DM::Error) << "Negative pressure in pressure surface: StartP " << startP  << " EndP " << endP << " Length of pipe: " << length;
		}
		*/

		if(pressureDiff < 0 && flow > 0)
		{
			//DM::Logger(DM::Warning) << "Pipe with negative pressure difference: " << pressureDiff << " Flow: " << flow << " EPANET ID: " << (int)epanetID;
			//DM::Logger(DM::Warning) << "Please place a pump here or use more observed pressure points in your system to balance all reservoirs";
			diameter = converter->calcDiameter(roughness,TBVectorData::calculateDistance(currentpipe->getStartNode(),currentpipe->getEndNode()),flow/1000.0,std::fabs(pressureDiff),maxdiameter,discretediameter,nearestdiscretediameter);
		}
		else
		{
			diameter = converter->calcDiameter(roughness,TBVectorData::calculateDistance(currentpipe->getStartNode(),currentpipe->getEndNode()),flow/1000.0,pressureDiff,maxdiameter,discretediameter,nearestdiscretediameter);
		}

		if(std::find(fixedpipes.begin(),fixedpipes.end(),currentpipe)==fixedpipes.end())
		{
			if(usemainpipes && std::find(mainpipes.begin(),mainpipes.end(),currentpipe)!=mainpipes.end())
			{
				if(	std::find(reservoirs.begin(),reservoirs.end(),currentpipe->getStartNode()) == reservoirs.end() &&
					std::find(reservoirs.begin(),reservoirs.end(),currentpipe->getEndNode()) == reservoirs.end())
				{
					if(diameter < minmainpipediameter)
						diameter = minmainpipediameter;
				}
			}

			if(!converter->checkENRet(EPANET::ENsetlinkvalue(epanetID,EN_DIAMETER,diameter*1000)))return false;
			if(!converter->checkENRet(EPANET::ENsetlinkvalue(epanetID,EN_ROUGHNESS,roughness*1000)))return false;
			currentpipe->changeAttribute(wsd.getAttributeString(DM::WS::PIPE,DM::WS::PIPE_ATTR_DEF::Diameter),diameter);
			currentpipe->changeAttribute(wsd.getAttributeString(DM::WS::PIPE,DM::WS::PIPE_ATTR_DEF::Roughness),roughness*1000);
		}
	}

	if(!converter->checkENRet(EPANET::ENsolveH()))return false;

	return true;
}

bool Dimensioning::approximateMainPipes(bool usereservoirsdata, double totaldemand, std::vector<DM::Edge*> &respipes,bool discretediameters)
{
	double roughness = 0.0001; //roughness (m)
	double mindiameter = 0.2; //(m)
	std::vector<DM::Component*> reservoirs = this->sys->getAllComponentsInView(wsd.getCompleteView(WS::RESERVOIR,DM::READ));
	std::vector<DM::Component*> mainpipe = this->sys->getAllComponentsInView(wsd.getCompleteView(WS::MAINPIPE,DM::READ));
	std::map<DM::Component*, DM::Edge*> entrypipes;

	std::vector<double> supplypercent;
	double defaultdiameter = converter->calcDiameter(roughness,1000,totaldemand*0.75,1,maxdiameter/1000.0,discretediameters,nearestdiscretediameter) * 1000;

	if(defaultdiameter == 0)
		defaultdiameter=mindiameter;

	for(int index=0; index<reservoirs.size(); index++)
		if(usereservoirsdata)
			supplypercent.push_back(reservoirs[index]->getAttribute(wsd.getAttributeString(DM::WS::RESERVOIR,DM::WS::RESERVOIR_ATTR_DEF::SupplyPercent))->getDouble()/100.0);
		else
			supplypercent.push_back(1.0/reservoirs.size());

	for(int index = 0; index < mainpipe.size(); index++)
	{
		DM::Edge* currentedge = dynamic_cast<DM::Edge*>(mainpipe[index]);

		if(std::find(reservoirs.begin(),reservoirs.end(),currentedge->getStartNode())!=reservoirs.end())
			entrypipes[currentedge->getStartNode()] = currentedge;

		if(std::find(reservoirs.begin(),reservoirs.end(),currentedge->getEndNode())!=reservoirs.end())
			entrypipes[currentedge->getEndNode()] = currentedge;

		if(std::find(fixedpipes.begin(),fixedpipes.end(),currentedge)==fixedpipes.end())
		{
			currentedge->changeAttribute(wsd.getAttributeString(DM::WS::PIPE,DM::WS::PIPE_ATTR_DEF::Diameter),defaultdiameter);
			currentedge->changeAttribute(wsd.getAttributeString(DM::WS::PIPE,DM::WS::PIPE_ATTR_DEF::Roughness),roughness*1000);
		}
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
	int diameter[] = { 80, 100, 125, 150, 200, 250, 300, 350, 400, 500, 600, 800, 1000, 1500, 2000, 4000, 8000};
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
		if(diameter < 0)
			diameter = 10;
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
				diameter = 0.1;

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
