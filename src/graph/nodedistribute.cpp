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

#include <nodedistribute.h>
#include <examinationroomviewdef.h>
#include <tbvectordata.h>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real_distribution.hpp>
#include <boost/random/normal_distribution.hpp>

DM_DECLARE_NODE_NAME(DistributeNodes,Graph)

DistributeNodes::DistributeNodes()
{
	//init values
	uniform = true;
	normal = false;
	nodesum=0;
	nodeweight=0;

	//Define model input
	DM::ER::ViewDefinitionHelper defhelper_er;
	std::vector<DM::View> views;

	//Possible edges and nodes
	views.push_back(defhelper_graph.getCompleteView(DM::GRAPH::NODES,DM::WRITE));

	//examination room
	views.push_back(defhelper_er.getView(DM::ER::EXAMINATIONROOM,DM::READ));

	this->addData("Layout", views);

	this->addParameter("Node weight sum", DM::DOUBLE, &this->nodesum);
	this->addParameter("Weight of one node", DM::DOUBLE, &this->nodeweight);
	this->addParameter("Uniform distribute nodes", DM::BOOL, &this->uniform);
	this->addParameter("Normal distribute nodes", DM::BOOL, &this->normal);
}

void DistributeNodes::run()
{
	//Typedefs
	typedef std::pair<std::string,DM::Component*> Cp;
	typedef std::vector<DM::Component*>::iterator CompItr;

	//Define vars
	DM::ER::ViewDefinitionHelper defhelper_er;

	//Get System information
	this->sys = this->getData("Layout");
	std::vector<DM::Component*> fm = sys->getAllComponentsInView(defhelper_er.getView(DM::ER::EXAMINATIONROOM,DM::READ));

	if(fm.size() < 1)
	{
		DM::Logger(DM::Error) << "Could not find any examination room";
		return;
	}

	//calculate boundingbox (There exists a methode in dynamind extensions which also calculates a bounding box.
	//In this case we want a bounding box which is normal orianted to x and y axis)
	double boundaries[4] = {0,0,0,0}; //(xmin,xmax,ymin,ymax)
	bool initboundaries=true;

	for(CompItr i = fm.begin(); i != fm.end(); ++i)
	{
		DM::Face* currentface = static_cast<DM::Face*>((*i));
		std::vector<DM::Node*> facenodes = currentface->getNodePointers();

		for(uint index=0; index < facenodes.size(); index++)
		{
			DM::Node* currentnode = facenodes[index];

			if(initboundaries)
			{
				initboundaries=false;
				boundaries[0] = currentnode->getX();
				boundaries[1] = boundaries[0];
				boundaries[2] = currentnode->getY();
				boundaries[3] = boundaries[2];
			}
			else
			{
				if(boundaries[0] > currentnode->getX())
					boundaries[0] = currentnode->getX();

				if(boundaries[1] < currentnode->getX())
					boundaries[1] = currentnode->getX();

				if(boundaries[2] > currentnode->getY())
					boundaries[2] = currentnode->getY();

				if(boundaries[3] < currentnode->getY())
					boundaries[3] = currentnode->getY();
			}
		}
	}

	//create nodes
	boost::mt19937 rng(time(NULL));
	double currentsum = 0;
	DM::Logger(DM::Standard) << "Sum of node weights: " << nodesum;
	DM::Logger(DM::Standard) << "Weight of one node: " << nodeweight;
	DM::Logger(DM::Standard) << "Normal distribution: " << normal;
	DM::Logger(DM::Standard) << "Uniform distribution: " << uniform;

	while(currentsum < nodesum)
	{
		double x=0, y=0;

		if(uniform)
		{
			x = getUniformDist(boundaries[0],boundaries[1],rng);
			y = getUniformDist(boundaries[2],boundaries[3],rng);
		}

		if(normal)
		{
			x = getNormalDist(boundaries[0],boundaries[1],rng);
			y = getNormalDist(boundaries[2],boundaries[3],rng);
		}

		DM::Node currentnode(x,y,0);
		currentnode.addAttribute(defhelper_graph.getAttributeString(DM::GRAPH::NODES,DM::GRAPH::NODES_ATTR_DEF::Weight),nodeweight);

		if(TBVectorData::PointWithinAnyFace(fm,&currentnode))
		{
			sys->addNode(currentnode,defhelper_graph.getCompleteView(DM::GRAPH::NODES,DM::WRITE));
			currentsum+=nodeweight;
		}
	}

	DM::Logger(DM::Standard) << "Generated sum of node weights: " << currentsum;
}


double DistributeNodes::getNormalDist(double min, double max, boost::mt19937 &rng)
{
	double mean = (max -min)/2 + min;
	boost::random::normal_distribution<> rand(mean, (max - min)/6);
	return rand(rng);
}

double DistributeNodes::getUniformDist(double min, double max, boost::mt19937 &rng)
{
	boost::random::uniform_real_distribution<> rand(min,max);
	return rand(rng);
}
