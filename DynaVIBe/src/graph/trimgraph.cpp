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

#include <trimgraph.h>
#include <examinationroomviewdef.h>
#include <tbvectordata.h>

DM_DECLARE_NODE_NAME(TrimGraph,Graph)

TrimGraph::TrimGraph()
{
	//Define model input
	DM::ER::ViewDefinitionHelper defhelper_er;
	std::vector<DM::View> views;

	//Possible edges and nodes
	views.push_back(defhelper_graph.getView(DM::GRAPH::EDGES,DM::MODIFY));
	views.push_back(defhelper_graph.getView(DM::GRAPH::NODES,DM::MODIFY));

	//examination room
	views.push_back(defhelper_er.getView(DM::ER::EXAMINATIONROOM,DM::READ));

	this->addData("Layout", views);
}

void TrimGraph::run()
{
	//Typedefs
	typedef std::pair<std::string,DM::Component*> Cp;
	typedef std::vector<DM::Component*>::iterator CompItr;

	//Define vars
	DM::ER::ViewDefinitionHelper defhelper_er;

	//Get System information
	this->sys = this->getData("Layout");
	std::vector<DM::Component*> em = sys->getAllComponentsInView(defhelper_graph.getView(DM::GRAPH::EDGES,DM::MODIFY));
	std::vector<DM::Component*> nm = sys->getAllComponentsInView(defhelper_graph.getView(DM::GRAPH::NODES,DM::MODIFY));
	std::vector<DM::Component*> fm = sys->getAllComponentsInView(defhelper_er.getView(DM::ER::EXAMINATIONROOM,DM::READ));

	for(CompItr i = em.begin(); i!=em.end(); i++)
	{
		DM::Edge* currentedge = static_cast<DM::Edge*>((*i));

		if(!TBVectorData::EdgeWithinAnyFace(fm,currentedge))
		{
			sys->removeComponentFromView(currentedge,defhelper_graph.getView(DM::GRAPH::EDGES,DM::MODIFY));

			if(std::find(nm.begin(),nm.end(),currentedge->getStartNode()) != nm.end())
				if(!TBVectorData::PointWithinAnyFace(fm,currentedge->getStartNode()))
					sys->removeComponentFromView(currentedge->getStartNode(),defhelper_graph.getView(DM::GRAPH::NODES,DM::MODIFY));

			if(std::find(nm.begin(),nm.end(),currentedge->getEndNode()) != nm.end())
				if(!TBVectorData::PointWithinAnyFace(fm,currentedge->getEndNode()))
					sys->removeComponentFromView(currentedge->getEndNode(),defhelper_graph.getView(DM::GRAPH::NODES,DM::MODIFY));
		}
	}
}
