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

#include <reducetree.h>

//DynaMind includes
#include <dmsystem.h>
#include <dmlogsink.h>
#include <math.h>

//boost
#include <boost/bind.hpp>

DM_DECLARE_NODE_NAME(ReduceTree,Graph)

ReduceTree::ReduceTree()
{
	std::vector<DM::View> views;
	DM::View view;
	DM::GRAPH::ViewDefinitionHelper defhelper;

	view = defhelper.getCompleteView(DM::GRAPH::EDGES,DM::MODIFY);
	views.push_back(view);
	viewdef[DM::GRAPH::EDGES]=view;

	view = defhelper.getView(DM::GRAPH::NODES,DM::MODIFY);
	views.push_back(view);
	viewdef[DM::GRAPH::NODES]=view;

	view = DM::View("FORCEDNODES", DM::NODE, DM::READ);
	views.push_back(view);

	this->addData("Layout", views);
}

void ReduceTree::run()
{
	this->sys = this->getData("Layout");
	typedef std::map<std::string,DM::Component*>::iterator itr;
	typedef std::map<std::string,DM::Component*> CMap;
	typedef std::vector<DM::Component*> CVec;
	CMap edges = sys->getAllComponentsInView(viewdef[DM::GRAPH::EDGES]);
	CMap nodes = sys->getAllComponentsInView(viewdef[DM::GRAPH::NODES]);
	CMap forcednodes = sys->getAllComponentsInView(DM::View("FORCEDNODES", DM::NODE, DM::READ));
	int deletecounter = 0;
	CVec nodesvec;

	std::transform( nodes.begin(), nodes.end(),std::back_inserter(nodesvec),boost::bind(&CMap::value_type::second,_1) );

	QMultiMap<DM::Node*, DM::Edge*> graph;

	DM::Logger(DM::Standard) << "Init out/in edges";

    #pragma omp parallel for
	for(int i = 0; i < nodesvec.size(); i++)
	{
		DM::Node *currentnode = static_cast<DM::Node*>(nodesvec[i]);
		std::vector<DM::Edge *> connectededges = getEdgesInView(currentnode,edges);

		for(uint index=0; index < connectededges.size(); index++)
		{
			#pragma omp critical
			{
				graph.insert(currentnode, connectededges[index]);
			}
		}
	}

	DM::Logger(DM::Standard) << "Reduceing tree";
	DM::Logger(DM::Standard) << "Grahp has " << edges.size() << " edges";

	//TODO CONTAINS A BUG --- DOES NOT DELETE THE LAST ELEMENT
	for(uint i = 0; i < nodesvec.size(); i++)
	{
		DM::Node *currentnode = static_cast<DM::Node*>(nodesvec[i]);

        while(graph.count(currentnode) == 1 && (forcednodes.find(currentnode->getUUID()) == forcednodes.end()))
		{
			DM::Node* endnode = 0;
			DM::Edge* currentedge = graph.find(currentnode).value();

			if(currentedge->getStartNode()==currentnode)
				endnode = currentedge->getEndNode();

			if(currentedge->getEndNode()==currentnode)
				endnode = currentedge->getStartNode();

			if(!endnode)
			{
				DM::Logger(DM::Error) << "Could not find node within an edge which should exist";
				break;
			}

			sys->removeComponentFromView(currentedge,viewdef[DM::GRAPH::EDGES]);
			sys->removeComponentFromView(currentnode,viewdef[DM::GRAPH::NODES]);

			graph.remove(currentnode,currentedge);
			graph.remove(endnode,currentedge);
			currentnode = endnode;
			deletecounter++;
		}
	}

	DM::Logger(DM::Standard) << "Removed " << deletecounter << " edges from tree";

	return;
}


std::vector<DM::Edge *> ReduceTree::getEdgesInView(DM::Node* node, std::map<std::string, DM::Component*> &edges)
{
	std::vector<DM::Edge *> all = node->getEdges();
	std::vector<DM::Edge *> result;

	for(uint index=0; index < all.size(); index++)
	{
		DM::Edge* currentedge = all[index];

		if(edges.find(currentedge->getUUID()) != edges.end())
			result.push_back(all[index]);
	}

	return result;
}
