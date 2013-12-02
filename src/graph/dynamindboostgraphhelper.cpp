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

#include <dynamindboostgraphhelper.h>
#include <graphviewdef.h>
#include <tbvectordata.h>

bool DynamindBoostGraph::createBoostGraph(Compmap &nodes,Compmap &edges,Graph &g,std::map<DM::Node*,int> &nodesindex,std::map<std::pair < int, int >, DM::Edge*> &nodes2edge)
{
	DM::GRAPH::ViewDefinitionHelper defhelper_graph;
	g.clear();
	int nodeindex=0;
	for(Compitr itr = nodes.begin(); itr!=nodes.end(); ++itr)
	{
		nodesindex[static_cast<DM::Node*>((*itr))]=nodeindex;
		add_vertex(nodeindex,g);
		nodeindex++;
	}


	for(Compitr itr = edges.begin(); itr!=edges.end(); ++itr)
	{
		int sourceindex, targetindex;
		DM::Edge *edge = static_cast<DM::Edge*>((*itr));

		double distance = edge->getAttribute(defhelper_graph.getAttributeString(DM::GRAPH::EDGES,DM::GRAPH::EDGES_ATTR_DEF::Weight))->getDouble();

		sourceindex = nodesindex[edge->getStartNode()];
		targetindex = nodesindex[edge->getEndNode()];

		nodes2edge[std::make_pair(sourceindex,targetindex)]=edge;
		add_edge(sourceindex, targetindex, distance, g);
	}

	return true;
}

void DynamindBoostGraph::subtractGraphs(Compmap &a, Compmap &b)
{
	for(Compitr itr = b.begin(); itr != b.end(); ++itr)
	{
		DM::Edge* currentedge = static_cast<DM::Edge*>((*itr));
		a.erase(std::find(a.begin(),a.end(),currentedge));
	}
}

void DynamindBoostGraph::createSkeletonizeBoostGraph(Compmap &nodes, Compmap &edges, DynamindBoostGraph::Graph &Graph, std::map<DM::Node *, int> &nodesindex, std::map<std::pair<int, int>, std::vector<DM::Edge *> > &nodes2edge)
{
	DM::Logger(DM::Error) << "createSkeletonizeBoostGraph Not implemented";
	return;
}

//TODO multiple entries
bool DynamindBoostGraph::findShortestPath(std::vector<DM::Node*> &pathnodes,
								   std::vector<DM::Edge*> &pathedges,
								   double &distance,
								   std::map<DM::Node*,int> &nodesindex,
								   std::map<std::pair < int, int >, DM::Edge*> &nodes2edge,
								   boost::property_map<Graph, boost::vertex_distance_t>::type &distancevector,
								   std::vector<int> &predecessors,
								   DM::Node* root, DM::Node* target)
{
	distance=0;
	DM::Node *targetjunction = target;
	DM::Node *rootjunction = root;
	int tindex = nodesindex[targetjunction];
    int rindex = nodesindex[rootjunction];

	if(targetjunction==rootjunction)
		return false;

    //pathnodes.push_back(target);
    std::vector<int> path;
    path.push_back(tindex);

	while( predecessors[tindex] != tindex &&  path[path.size()-1] != rindex)
	{
		tindex = predecessors[tindex];
		path.push_back(tindex);
	}

	if(path[path.size()-1] != rindex)
		return false;

	for(uint s=0; s < path.size()-1; s++)
	{
		int sindex=path[s];
		tindex=path[s+1];

		DM::Edge *newpipe = 0;
        DM::Node *targetjunction;
        DM::Node *sourcejunction;

		if(nodes2edge.find(std::make_pair(sindex,tindex))!=nodes2edge.end())
        {
			newpipe = nodes2edge[std::make_pair(sindex,tindex)];
            sourcejunction = newpipe->getStartNode();
            targetjunction = newpipe->getEndNode();
        }

		if(nodes2edge.find(std::make_pair(tindex,sindex))!=nodes2edge.end())
        {
			newpipe = nodes2edge[std::make_pair(tindex,sindex)];
            sourcejunction = newpipe->getEndNode();
            targetjunction = newpipe->getStartNode();
        }

		if(!newpipe)
		{
            DM::Logger(DM::Warning) << "Could not find specific edge in system (ERROR ?)";
			return false;
		}

		distance += TBVectorData::calculateDistance(sourcejunction,targetjunction);

        if(s==0)
            pathnodes.push_back(sourcejunction);

		pathnodes.push_back(targetjunction);
		pathedges.push_back(newpipe);
	}

	return true;
}
