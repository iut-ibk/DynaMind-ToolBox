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

#include <evaluateperformanceindicators.h>

//DynaMind includes
#include <dmsystem.h>
#include <dmlogsink.h>
#include <math.h>
#include <tbvectordata.h>
#include <list>
#include <cmath>

using namespace DM;

DM_DECLARE_NODE_NAME(EvaluatePerformanceIndicators,Watersupply-Performance)

EvaluatePerformanceIndicators::EvaluatePerformanceIndicators()
{   
	filepath = "";
	cost=false;
	pressure=false;
	resilience=false;

	this->addParameter("Cost", DM::BOOL, &this->cost);
	this->addParameter("Pressure", DM::BOOL, &this->pressure);
	this->addParameter("Resilience", DM::BOOL, &this->resilience);

	this->addParameter("Result file path", DM::FILENAME, &this->filepath);

    std::vector<DM::View> views;
	views.push_back(wsd.getCompleteView(WS::JUNCTION,DM::MODIFY));
	views.push_back(wsd.getCompleteView(WS::PIPE,DM::MODIFY));
	this->addData("Watersupply", views);
}

void EvaluatePerformanceIndicators::run()
{
	QString resultheader = "";
	QString result = "";
	typedef std::vector<DM::Component*> cmap;
    cmap::iterator itr;

	this->sys = this->getData("Watersupply");

	cmap junctions = sys->getAllComponentsInView(wsd.getView(DM::WS::JUNCTION, DM::MODIFY));
	cmap pipes = sys->getAllComponentsInView(wsd.getView(DM::WS::PIPE, DM::MODIFY));

	uint nj = junctions.size();
	uint np = pipes.size();


	//general characteristics
	double totallength;
	for(uint index=0; index < pipes.size(); index++)
	{
		DM::Edge *currentedge = dynamic_cast<DM::Edge*>(pipes[index]);
		totallength += TBVectorData::calculateDistance(currentedge->getStartNode(), currentedge->getEndNode());
	}

	resultheader+="#junctions,#pipes,Totallength,";
	result += QString::number(nj,'f',0) + "," + QString::number(np,'f',0) + "," + QString::number(totallength,'f',4) + ",";

	if(cost)
	{
		std::map<int,double> anualpipecosts;
		std::map<int,double> constructioncost;
		typedef std::map<int,double>::iterator mit;

		//USD
		constructioncost[80] = 60;
		constructioncost[100] = 70;
		constructioncost[150] = 90;
		constructioncost[200] = 130;
		constructioncost[300] = 180;
		constructioncost[400] = 260;
		constructioncost[500] = 310;
		constructioncost[600] = 360;


		//anualcost including construction and operation USD
		anualpipecosts[102]=9.97;
		anualpipecosts[151]=12.1;
		anualpipecosts[203]=14.49;
		anualpipecosts[254]=15.55;
		anualpipecosts[305]=18.28;
		anualpipecosts[356]=19.94;
		anualpipecosts[406]=23.26;
		anualpipecosts[457]=26.65;
		anualpipecosts[508]=29.58;
		anualpipecosts[610]=42.8;
		anualpipecosts[711]=48.12;
		anualpipecosts[762]=51.11;

		resultheader+="Cost,Anualcost,";
		double totalcost=0.0;
		double anualcost=0.0;

		for(uint index=0; index < pipes.size(); index++)
		{
			DM::Edge *currentedge = dynamic_cast<DM::Edge*>(pipes[index]);
			double length = TBVectorData::calculateDistance(currentedge->getStartNode(), currentedge->getEndNode());
			double diameter = currentedge->getAttribute(wsd.getAttributeString(DM::WS::PIPE,DM::WS::PIPE_ATTR_DEF::Diameter))->getDouble();
			double currentcost = constructioncost.begin()->second;

			for(mit it = constructioncost.begin(); it != constructioncost.end(); it++)
			{
				currentcost = it->second;
				if(it->first > diameter)
					break;
			}

			totalcost+=currentcost*length;
		}

		for(uint index=0; index < pipes.size(); index++)
		{
			DM::Edge *currentedge = dynamic_cast<DM::Edge*>(pipes[index]);
			double length = TBVectorData::calculateDistance(currentedge->getStartNode(), currentedge->getEndNode());
			double diameter = currentedge->getAttribute(wsd.getAttributeString(DM::WS::PIPE,DM::WS::PIPE_ATTR_DEF::Diameter))->getDouble();
			double currentcost = anualpipecosts.begin()->second;

			for(mit it = anualpipecosts.begin(); it != anualpipecosts.end(); it++)
			{
				currentcost = it->second;
				if(it->first > diameter)
					break;
			}

			anualcost+=currentcost*length;
		}

		result += QString::number(totalcost,'f',4) + "," + QString::number(anualcost,'f',4) + ",";
	}

	if(pressure)
	{
		resultheader+="Pressure,";
		double minpress = 20;
		double maxpress = 100;
		double totaldemand=0.0;
		double validdemand=0.0;

		for(uint index=0; index < junctions.size(); index++)
		{
			DM::Component *currentnode = junctions[index];
			double currentdemand = currentnode->getAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Demand))->getDouble();
			double currentpressure = currentnode->getAttribute(wsd.getAttributeString(DM::WS::JUNCTION,DM::WS::JUNCTION_ATTR_DEF::Pressure))->getDouble();
			totaldemand+=currentdemand;

			if(currentpressure>=minpress && currentpressure<=maxpress)
				validdemand+=currentdemand;
		}

		double ppi = validdemand/totaldemand;
		result += QString::number(ppi,'f',4) + ",";
	}

	if(resilience)
	{
		resultheader+="k,rm,";
		double k = (2*np)/double(nj);
		result += QString::number(k,'f',4) + ",";

		double rm = (np-nj+1)/double(2*nj-5);
		result += QString::number(rm,'f',4) + ",";
	}

	DM::Logger(DM::Standard) << resultheader;
	DM::Logger(DM::Standard) << result;


	//write results to file
	bool writeheader=false;
	if(!QFile::exists(QString::fromStdString(filepath)))
		writeheader=true;

	QFile file(QString::fromStdString(filepath));
	if(!file.open(QFile::Append | QFile::Text))
		return;

	QTextStream out(&file);
	if(writeheader)
		out << resultheader << "\n";

	out << result << "\n";
	file.close();
}
