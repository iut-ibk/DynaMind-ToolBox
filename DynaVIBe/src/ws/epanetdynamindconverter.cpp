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

#include <epanetdynamindconverter.h>

//DynaMind includes
#include <dmsystem.h>
#include <dmlogsink.h>
#include <math.h>
#include <cmath>
#include <tbvectordata.h>

//Watersupply
#include <dmepanet.h>

using namespace DM;

EpanetDynamindConverter::EpanetDynamindConverter()
{
	openedepanetfile=false;
}

EPANETModelCreator *EpanetDynamindConverter::getCreator()
{
	return &creator;
}

bool EpanetDynamindConverter::createEpanetModel(System *sys, string inpfilepath)
{
	DM::WS::ViewDefinitionHelper wsd;
	typedef std::vector<DM::Component*> cmap;
	cmap::iterator itr;

	//SET OPTIONS
	if(!creator.setOptionUnits(EPANETModelCreator::LPS))return false;
	if(!creator.setOptionHeadloss(EPANETModelCreator::DW))return false;

	//JUNCTIONS
	cmap junctions = sys->getAllComponentsInView(wsd.getView(DM::WS::JUNCTION, DM::READ));

	for(itr = junctions.begin(); itr != junctions.end(); ++itr)
		if(!addJunction(static_cast<DM::Node*>((*itr))))return false;

	//RESERVOIRS
	cmap reservoir = sys->getAllComponentsInView(wsd.getView(DM::WS::RESERVOIR, DM::READ));

	for(itr = reservoir.begin(); itr != reservoir.end(); ++itr)
		if(!addReservoir(static_cast<DM::Node*>((*itr))))return false;

	//TANKS
	cmap tank = sys->getAllComponentsInView(wsd.getView(DM::WS::TANK, DM::READ));

	for(itr = tank.begin(); itr != tank.end(); ++itr)
		if(!addTank(static_cast<DM::Node*>((*itr))))return false;

	//PIPES
	cmap pipes = sys->getAllComponentsInView(wsd.getView(DM::WS::PIPE, DM::READ));

	for(itr = pipes.begin(); itr != pipes.end(); ++itr)
	{
		bool cv = false;
		DM::Edge *pipe = static_cast<DM::Edge*>((*itr));
		if(std::find(reservoir.begin(),reservoir.end(),pipe->getStartNode()) != reservoir.end() || std::find(reservoir.begin(),reservoir.end(),pipe->getEndNode()) != reservoir.end())
			cv = true;

		if(!addPipe(pipe,cv))return false;
	}


	if(!creator.save(inpfilepath))return false;

	return true;
}

bool EpanetDynamindConverter::mapEpanetAttributes(System *sys)
{
	//TODO map more attributes (currently only the diameter of a pipe is extracted)
	if(!mapPipeAttributes(sys))
		return false;

	if(!mapJunctionAttributes(sys))
		return false;

	return true;
}

bool EpanetDynamindConverter::mapPipeAttributes(System *sys)
{
	//TODO map more attributes (currently only the diameter of a pipe is extracted)
	if(!openedepanetfile)
		return false;

	DM::WS::ViewDefinitionHelper wsd;
	typedef std::vector<DM::Component*> cmap;
	cmap::iterator itr;

	cmap pipes = sys->getAllComponentsInView(wsd.getView(DM::WS::PIPE, DM::MODIFY));

	for(itr = pipes.begin(); itr != pipes.end(); ++itr)
	{
		DM::Edge *currentedge = static_cast<DM::Edge*>((*itr));
		float currentdiameter;
		float roughness;
		float flow;
		float status;
		char name[256];
		strcpy(name, QString::number(components[currentedge]).toStdString().c_str());
		int index;
		if(!checkENRet(EPANET::ENgetlinkindex(name, &index)))
			return false;

		if(!checkENRet(EPANET::ENgetlinkvalue(index,EN_ROUGHNESS,&roughness)))
			return false;

		currentedge->changeAttribute(wsd.getAttributeString(DM::WS::PIPE,DM::WS::PIPE_ATTR_DEF::Roughness),roughness);

		if(!checkENRet(EPANET::ENgetlinkvalue(index,EN_DIAMETER,&currentdiameter)))
			return false;

		currentedge->changeAttribute(wsd.getAttributeString(DM::WS::PIPE,DM::WS::PIPE_ATTR_DEF::Diameter),currentdiameter);

		if(!checkENRet(EPANET::ENgetlinkvalue(index,EN_FLOW,&flow)))
			return false;

		currentedge->changeAttribute(wsd.getAttributeString(DM::WS::PIPE,DM::WS::PIPE_ATTR_DEF::Flow),flow);

		if(!checkENRet(EPANET::ENgetlinkvalue(index,EN_STATUS,&status)))
			return false;

		bool error;
		std::string stringstatus = EPANETModelCreator::convertPipeStatusToString(EPANETModelCreator::PIPESTATUS(int(status)),error);

		int linktype;
		if(!checkENRet(EPANET::ENgetlinktype(index,&linktype)))
			return false;

		if(linktype==EN_CVPIPE)
			stringstatus = EPANETModelCreator::convertPipeStatusToString(EPANETModelCreator::CV,error);

		currentedge->changeAttribute(wsd.getAttributeString(DM::WS::PIPE,DM::WS::PIPE_ATTR_DEF::STATUS),stringstatus);
	}

	return true;
}

bool EpanetDynamindConverter::mapJunctionAttributes(System *sys)
{
	//TODO map more attributes (currently only the pressure of a junction is extracted)
	if(!openedepanetfile)
		return false;

	DM::WS::ViewDefinitionHelper wsd;
	typedef std::vector<DM::Component*> cmap;
	cmap::iterator itr;

	cmap junctions = sys->getAllComponentsInView(wsd.getView(DM::WS::JUNCTION, DM::MODIFY));

	for(itr = junctions.begin(); itr != junctions.end(); ++itr)
	{
		DM::Node *currentnode = static_cast<DM::Node*>((*itr));
		float currentpressure;
		float currenttotalhead;
		char name[256];
		strcpy(name, QString::number(components[currentnode]).toStdString().c_str());
		int index;
		if(!checkENRet(EPANET::ENgetnodeindex(name, &index)))
			return false;

		if(!checkENRet(EPANET::ENgetnodevalue(index,EN_PRESSURE,&currentpressure)))
			return false;

		if(!checkENRet(EPANET::ENgetnodevalue(index,EN_HEAD,&currenttotalhead)))
			return false;

		currentnode->changeAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure),currentpressure);
		currentnode->changeAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::TotalHead),currenttotalhead);
	}

	return true;
}

bool EpanetDynamindConverter::checkENRet(int ret)
{
	if(!openedepanetfile)
	{
		DM::Logger(DM::Error) << "Please open EPANET model with EpanetDynamindConverter";
		return false;
	}

	if(ret>0)
	{
		const uint SIZE = 100;
		char errorstring[SIZE];
        int newerr = EPANET::ENgeterror(ret,errorstring,SIZE);

		if(ret > 7)
		{
			DM::Logger(DM::Error) << "EPANET error code: " << ret << " (" << errorstring << ")";
            if(newerr != 251)
                checkENRet(newerr);
			return false;
		}
		else
		{
			DM::Logger(DM::Warning) << "EPANET warning code: " << ret << " (" << errorstring << ")";
			return true;
		}
	}

	return true;
}

bool EpanetDynamindConverter::addJunction(DM::Node *junction)
{
	double x, y , elevation, demand;
	std::string tag;
	x = junction->getX();
	y = junction->getY();
	elevation = junction->getZ();
	demand = junction->getAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Demand))->getDouble();
	tag = junction->getAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Tag))->getString();

	uint index = creator.addJunction(x,y,elevation,demand,"",tag);

	if(!index)
		return false;

	components[junction]=index;

	return true;
}

bool EpanetDynamindConverter::addReservoir(DM::Node *reservoir)
{
	double x = reservoir->getX();
	double y = reservoir->getY();
	double head = reservoir->getZ();
	std::string headpattern = "";

	uint index = creator.addReservoir(x,y,head,headpattern);

	if(!index)
		return false;

	components[reservoir]=index;

	return true;
}

bool EpanetDynamindConverter::addTank(DM::Node *tank)
{
	double x = tank->getX();
	double y = tank->getY();
	double bottomelevation = tank->getZ();
	double initiallevel = 5;
	double minlevel = 0;
	double maxlevel = 5;
	double nominaldiameter = 10;
	double minvolume = 0;
	std::string volumecurve = "";

	uint index = creator.addTank(x,y,bottomelevation,initiallevel,minlevel,maxlevel,nominaldiameter,minvolume,volumecurve);

	if(!index)
		return false;

	components[tank]=index;

	return true;
}

bool EpanetDynamindConverter::addPipe(DM::Edge *pipe, bool cv)
{
	if(!components.contains(pipe->getStartNode()) || !components.contains(pipe->getEndNode()))
	{
		DM::Logger(DM::Error) << "Cannot add pipe because some nodes are not added";
		return false;
	}

	int startnode = components[pipe->getStartNode()];
	int endnode = components[pipe->getEndNode()];

	double length = pipe->getAttribute(wsd.getAttributeString(DM::WS::PIPE,DM::WS::PIPE_ATTR_DEF::Length))->getDouble();
	double diameter = pipe->getAttribute(wsd.getAttributeString(DM::WS::PIPE,DM::WS::PIPE_ATTR_DEF::Diameter))->getDouble();
	double roughness = pipe->getAttribute(wsd.getAttributeString(DM::WS::PIPE,DM::WS::PIPE_ATTR_DEF::Roughness))->getDouble();
	double minorloss = pipe->getAttribute(wsd.getAttributeString(DM::WS::PIPE,DM::WS::PIPE_ATTR_DEF::Minorloss))->getDouble();
	std::string tag = pipe->getAttribute(wsd.getAttributeString(DM::WS::PIPE,DM::WS::PIPE_ATTR_DEF::Tag))->getString();
	std::string string_status = pipe->getAttribute(wsd.getAttributeString(DM::WS::PIPE,DM::WS::PIPE_ATTR_DEF::STATUS))->getString();
	bool statuserror = false;
	EPANETModelCreator::PIPESTATUS status = creator.convertStringToPipeStatus(string_status,statuserror);

	if(cv)
		status = EPANETModelCreator::CV;

	uint index = creator.addPipe(startnode, endnode,length,diameter,roughness,minorloss,status,tag);

	if(!index)
		return false;

	if(!creator.addVertex(pipe->getStartNode()->getX(),
							 pipe->getStartNode()->getY(),
							 pipe->getEndNode()->getX(),
							 pipe->getEndNode()->getY(),QString::number(index)))
		return false;

	components[pipe]=index;

	return true;
}

EpanetDynamindConverter::~EpanetDynamindConverter()
{
	if(openedepanetfile)
	{
		EPANET::ENcloseH();
		EPANET::ENclose();
	}
}

bool EpanetDynamindConverter::openEpanetModel(string inpfilepath, string rptfilepath)
{
	if(openedepanetfile)
	{
		EPANET::ENcloseH();
		EPANET::ENclose();
	}

	char inpfile[512];
	strcpy(inpfile,inpfilepath.c_str());
	char rptfile[512];
	strcpy(rptfile,rptfilepath.c_str());

	openedepanetfile=true;
	if(!checkENRet(EPANET::ENopen(inpfile,rptfile,"")))
	{
		openedepanetfile=false;
		return false;
	}

	return true;
}

bool EpanetDynamindConverter::closeEpanetModel()
{
	if(checkENRet(EPANET::ENclose()))
	{
		openedepanetfile=false;
		return true;
	}

	return false;
}

int EpanetDynamindConverter::getEpanetNodeID(Component *component)
{
	if(!components.contains(component))
		return -1;

	char name[256];
	strcpy(name, QString::number(components[component]).toStdString().c_str());
	int index;
	if(!checkENRet(EPANET::ENgetnodeindex(name, &index)))
		return -1;

	return index;
}

int EpanetDynamindConverter::getEpanetLinkID(Component *component)
{
	if(!components.contains(component))
		return -1;

	char name[256];
	strcpy(name, QString::number(components[component]).toStdString().c_str());
	int index;
	if(!checkENRet(EPANET::ENgetlinkindex(name, &index)))
		return -1;

	return index;
}

DM::Node* EpanetDynamindConverter::getNearestPressure(DM::Node* currentpressurepoint, std::vector<DM::Node*> &nodes)
{
	DM::Node* result = 0;
	if(getEpanetNodeID(currentpressurepoint) < 0)
		return result;

	nodes.push_back(currentpressurepoint);

	std::vector<DM::Edge*> edges = currentpressurepoint->getEdges();

	for(int index=0; index < edges.size(); index++)
	{
		DM::Edge* currentedge = edges[index];

		if(!components.contains(currentedge))
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

DM::Node* EpanetDynamindConverter::getNearestFlowPoint(DM::Node* currentpoint, std::vector<DM::Node*> &nodes)
{
	DM::Node* result = 0;
	if(!components.contains(currentpoint))
		return result;

	nodes.push_back(currentpoint);

	std::vector<DM::Edge*> edges = currentpoint->getEdges();

	for(int index=0; index < edges.size(); index++)
	{
		DM::Edge* currentedge = edges[index];

		if(!components.contains(currentedge))
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

std::vector<DM::Node*> EpanetDynamindConverter::getInverseFlowNeighbours(DM::Node* junction)
{
	//Downstream neighbours (Neigbours which are getting water of the current junction)
	std::vector<DM::Node*> result;

	if(!components.contains(junction))
		return result;


	std::vector<DM::Edge*> edgevec = junction->getEdges();

	for(int index=0; index < edgevec.size(); index++)
	{
		DM::Edge* currentedge = edgevec[index];

		if(!components.contains(currentedge))
			continue;

		int ID = getEpanetLinkID(currentedge);

		bool invert=true;

		if(currentedge->getStartNode()==junction)
			invert=false;

		float flow = 0.0;

		if(!checkENRet(EPANET::ENgetlinkvalue(ID,EN_FLOW,&flow)))return std::vector<DM::Node*>();

		if(std::fabs(flow) < 0.0001)
			flow=0.0;

		if(invert)
			flow = -flow;

		if(flow < 0)
			continue;

		if(currentedge->getStartNode()==junction)
			result.push_back(currentedge->getEndNode());
		else
			result.push_back(currentedge->getStartNode());
	}

	return result;
}

std::vector<DM::Node*> EpanetDynamindConverter::getFlowNeighbours(DM::Node* junction)
{
	//Upstream neighbours (Neigbours which supply water)
	std::vector<DM::Node*> result;

	if(!components.contains(junction))
		return result;

	std::vector<DM::Edge*> edgevec = junction->getEdges();

	for(int index=0; index < edgevec.size(); index++)
	{
		DM::Edge* currentedge = edgevec[index];

		if(!components.contains(currentedge))
			continue;

		int ID = getEpanetLinkID(currentedge);

		bool invert=true;

		if(currentedge->getStartNode()==junction)
			invert=false;

		float flow = 0.0;

		if(!checkENRet(EPANET::ENgetlinkvalue(ID,EN_FLOW,&flow)))
			return std::vector<DM::Node*>();

		if(std::fabs(flow) < 0.0001)
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

double EpanetDynamindConverter::calcDiameter(double k, double l, double q, double h, double maxdiameter, bool discretediameters, bool nearestdiscretediameter)
{
	double mindiameter = 0.05;
	std::vector<double> diameters;
	diameters.push_back(0.050);
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
		return mindiameter;

	if(h < 0)
		return -1;

	double maxerror = 0.0000001;
	double d = 0.001;
	double maxd = diameters[diameters.size()-1];
	double mind = 0;
	double currenterror = h-calcFrictionHeadLoss(d,k,l,q);

	if(h/l < 0.00001)
	{
		d =  std::sqrt(4*q)/M_PI;
	}

	if(d <= 0.001)
	{
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

	if(d < diameters[0] && discretediameters)
		d = diameters[0];

	return d;
}

double EpanetDynamindConverter::calcFrictionHeadLoss(double d, double k, double l, double q)
{
	double headloss = 0;

	headloss =
			calcLambda(k,d,q)*
			(l/d)*
			(std::pow(q/((std::pow(d,2)/4.0)*M_PI),2)/(2*9.81));

	return headloss;
}

double EpanetDynamindConverter::calcLambda(double k, double d, double q, double lambda)
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
