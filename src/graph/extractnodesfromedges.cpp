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

#include <extractnodesfromedges.h>

//DynaMind includes
#include <dmsystem.h>
#include <dmlogsink.h>

DM_DECLARE_NODE_NAME(ExtractNodesFromEdges,Graph)

ExtractNodesFromEdges::ExtractNodesFromEdges()
{
	std::vector<DM::View> views;
	DM::View view;
	DM::GRAPH::ViewDefinitionHelper defhelper;
	this->skeletonize = false;

	//Define Parameter street network
	view = defhelper.getView(DM::GRAPH::EDGES,DM::READ);
	views.push_back(view);
	viewdef[DM::GRAPH::EDGES]=view;

	//Define Parameter street network
	view = defhelper.getView(DM::GRAPH::NODES,DM::WRITE);
	views.push_back(view);
	viewdef[DM::GRAPH::NODES]=view;

	this->addData("Layout", views);

	this->addParameter("Skeletonize", DM::BOOL, &this->skeletonize);
}

void ExtractNodesFromEdges::run()
{
	this->sys = this->getData("Layout");

	std::map<std::string,DM::Component*> edges = sys->getAllComponentsInView(viewdef[DM::GRAPH::EDGES]);
	std::vector<DM::Component*> nodesadded;

	typedef std::map<std::string,DM::Component*>::iterator itr;

	DM::Logger(DM::Standard) << "Number of Edges found:" << edges.size();

	for(itr i = edges.begin(); i != edges.end(); i++)
	{
		DM::Edge *edge = static_cast<DM::Edge*>((*i).second);
		DM::Node* sname = edge->getStartNode();
		DM::Node* tname = edge->getEndNode();

		//SOURCE
		if(std::find(nodesadded.begin(),nodesadded.end(),sname)==nodesadded.end() && checkNode(sname,edges,this->skeletonize))
		{
			this->sys->addComponentToView(sname,viewdef[DM::GRAPH::NODES]);
			nodesadded.push_back(sname);
		}

		//TARGET
		if(std::find(nodesadded.begin(),nodesadded.end(), tname)==nodesadded.end() && checkNode(tname,edges,this->skeletonize))
		{
			this->sys->addComponentToView(tname,viewdef[DM::GRAPH::NODES]);
			nodesadded.push_back(tname);
		}
	}

	DM::Logger(DM::Standard) << "Number of extracted nodes: " << nodesadded.size();
}

bool ExtractNodesFromEdges::checkNode(DM::Node* node, std::map<std::string,DM::Component*> &edges, bool skeletonize)
{
	if(!skeletonize)
		return true;

	std::vector<DM::Edge*> e = node->getEdges();
	std::vector<DM::Edge*> possible_edges;

	for(uint index=0; index < e.size(); index++)
		if(edges.find(e[index]->getUUID())!=edges.end())
			possible_edges.push_back(e[index]);

	if(possible_edges.size() == 2)
		return false;

	return true;
}
