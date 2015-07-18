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

#include <identifymainpipes.h>

//DynaMind includes
#include <dmsystem.h>
#include <dmlogsink.h>
#include <tbvectordata.h>

#include <iostream>
#include <fstream>
#include <QFile>
#include <QTime>
#include <map>
#include <math.h>
#include <algorithm>
#include <numeric>

//BOOST GRAPH includes
//#include <boosttraits.h>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/random_spanning_tree.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <time.h>
#include <boost/make_shared.hpp>

#ifdef _OPENMP
#include <omp.h>
#endif

using namespace boost;

DM_DECLARE_NODE_NAME(IdentifyMainPipes,Watersupply)

IdentifyMainPipes::IdentifyMainPipes()
{
	//Define model input
	std::vector<DM::View> views;

	//ws system
	views.push_back(defhelper_ws.getView(DM::WS::SUGGESTEDMAINLOOPS, DM::READ));
	views.push_back(defhelper_ws.getView(DM::WS::JUNCTION, DM::READ));
	views.push_back(defhelper_ws.getView(DM::WS::PIPE, DM::READ));
	views.push_back(defhelper_ws.getView(DM::WS::RESERVOIR, DM::READ));
	views.push_back(defhelper_ws.getView(DM::WS::MAINPIPE, DM::WRITE));

	this->addData("Layout", views);
}

void IdentifyMainPipes::run()
{
	this->sys = this->getData("Layout");
	bool istree = false;
	//Define vars
	DynamindBoostGraph::Compmap jm = sys->getAllComponentsInView(defhelper_ws.getView(DM::WS::JUNCTION,DM::READ));
	DynamindBoostGraph::Compmap pm = sys->getAllComponentsInView(defhelper_ws.getView(DM::WS::PIPE,DM::READ));
	DynamindBoostGraph::Compmap rm = sys->getAllComponentsInView(defhelper_ws.getView(DM::WS::RESERVOIR,DM::READ));
	DynamindBoostGraph::Compmap mf = sys->getAllComponentsInView(defhelper_ws.getView(DM::WS::SUGGESTEDMAINLOOPS,DM::READ));
	DynamindBoostGraph::Compmap resultedges;
	DynamindBoostGraph::Compmap resultnodes;


	std::vector<DM::Component*> reducedgraphedges;
	std::vector<DM::Component*> reducedgraphnodes;

	for(int index=0; index < rm.size(); index++)
		jm.push_back(rm[index]);

	//extract looped graph
	if(!removeLeafs(jm,pm,reducedgraphedges, reducedgraphnodes))
		return;

	if(!reducedgraphedges.size())
	{
		istree=true;
		reducedgraphnodes=jm;
		reducedgraphedges=pm;
	}

	DynamindBoostGraph::Graph g;
	std::map<DM::Node*,int> nodesindex;
	std::map<std::pair < int, int >, DM::Edge*> nodes2edge;

	DynamindBoostGraph::createBoostGraph(reducedgraphnodes, reducedgraphedges,g,nodesindex,nodes2edge);

	//find nearest junctions
	for(int index=0; index < mf.size(); index++)
	{
		std::vector<DM::Node*> mappedJunctions;
		DM::Face* currentface = dynamic_cast<DM::Face*>(mf[index]);

		std::vector<DM::Node*> facenodes = currentface->getNodePointers();

		for(int fn=0; fn<facenodes.size(); fn++)
		{
			double distance = TBVectorData::calculateDistance(facenodes[fn],facenodes[(fn+1)%facenodes.size()]);
			if(distance < 100 && distance > 0)
				continue;

			std::vector<DM::Node*> candidates = TBVectorData::findNearestNeighbours(facenodes[fn],20000, reducedgraphnodes);

			if(candidates.size()==0)
			{
				DM::Logger(DM::Error) << "No candidates found";
				return;
			}

			mappedJunctions.push_back(candidates[0]);
		}

		//find pipe sequence with shortest path in loop
		for(int index=0; index < mappedJunctions.size(); index++)
		{
			std::vector<DM::Node*> pathnodes;
			std::vector<DM::Edge*> pathedges;
			std::vector < int > p(num_vertices(g));
			property_map<DynamindBoostGraph::Graph, vertex_distance_t>::type d = get(vertex_distance, g);
			double length;


			boost::dijkstra_shortest_paths( g,
											nodesindex[mappedJunctions[index]],
											predecessor_map(boost::make_iterator_property_map(p.begin(), get(boost::vertex_index, g)))
											.distance_map(d));

			DynamindBoostGraph::findShortestPath(pathnodes,
												 pathedges,
												 length,
												 nodesindex,
												 nodes2edge,
												 d,
												 p,
												 mappedJunctions[index],
												mappedJunctions[(index+1)%mappedJunctions.size()]);


			for(int pindex=0; pindex < pathedges.size(); pindex++)
			{
				DM::Edge* currentedge = pathedges[pindex];
				DM::Node* startnode = currentedge->getStartNode();
				DM::Node* endnode = currentedge->getEndNode();

				DynamindBoostGraph::Compmap::iterator edgeit = std::find(resultedges.begin(),resultedges.end(),currentedge);

				if(edgeit==resultedges.end())
					resultedges.push_back(currentedge);

				DynamindBoostGraph::Compmap::iterator nodeit = std::find(resultnodes.begin(),resultnodes.end(),startnode);

				if(nodeit==resultnodes.end())
					resultnodes.push_back(startnode);

				nodeit = std::find(resultnodes.begin(),resultnodes.end(),endnode);

				if(nodeit==resultnodes.end())
					resultnodes.push_back(endnode);
			}
		}

		DynamindBoostGraph::Compmap tmpresultedges = resultedges;
		DynamindBoostGraph::Compmap tmpresultnodes = resultnodes;

		if(!istree)
			removeLeafs(tmpresultnodes,tmpresultedges,resultedges,resultnodes);

	}

	for(int index=0; index < rm.size(); index++)
	{
		//if(std::find(jm.begin(),jm.end(),rm[index])==jm.end())
		//	jm.push_back(dynamic_cast<DM::Node*>(rm[index]));

		if(!addPathToNode(dynamic_cast<DM::Node*>(rm[index]),jm,pm,resultedges,resultnodes))
			DM::Logger(DM::Warning) << "Could not find a path from reservoir to main loop";
	}

	for(int index=0; index < resultedges.size(); index++)
		sys->addComponentToView(resultedges[index],defhelper_ws.getView(DM::WS::MAINPIPE, DM::WRITE));
}

void IdentifyMainPipes::initmodel()
{
}

std::vector<DM::Edge *> IdentifyMainPipes::getEdgesInView(DM::Node* node, std::vector<DM::Component*> &edges)
{
	std::vector<DM::Edge *> all = node->getEdges();
	std::vector<DM::Edge *> result;

	for(uint index=0; index < all.size(); index++)
	{
		DM::Edge* currentedge = all[index];

		if(std::find(edges.begin(),edges.end(),currentedge) != edges.end())
			result.push_back(all[index]);
	}

	return result;
}

bool IdentifyMainPipes::removeLeafs(std::vector<DM::Component *> nodes, std::vector<DM::Component *> edges, std::vector<DM::Component *> &resultedges, std::vector<DM::Component*> &resultnodes)
{
	QMultiMap<DM::Node*, DM::Edge*> graph;
	resultedges.clear();
	resultnodes.clear();

	for(int index=0; index < edges.size(); index++)
		resultedges.push_back(dynamic_cast<DM::Edge*>(edges[index]));

	for(int index=0; index < nodes.size(); index++)
		resultnodes.push_back(dynamic_cast<DM::Node*>(nodes[index]));

	#pragma omp parallel for
	for(int i = 0; i < nodes.size(); i++)
	{
		DM::Node *currentnode = dynamic_cast<DM::Node*>(nodes[i]);
		std::vector<DM::Edge *> connectededges = getEdgesInView(currentnode,edges);

		for(uint index=0; index < connectededges.size(); index++)
		{
			#pragma omp critical
			{
				graph.insert(currentnode, connectededges[index]);
			}
		}
	}

	for(uint i = 0; i < nodes.size(); i++)
	{
		DM::Node *currentnode = static_cast<DM::Node*>(nodes[i]);

		while(graph.count(currentnode) == 1)
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

			std::vector<DM::Component*>::iterator itedge = std::find(resultedges.begin(),resultedges.end(),currentedge);

			if(itedge != resultedges.end())
				resultedges.erase(itedge);

			std::vector<DM::Component*>::iterator itnode = std::find(resultnodes.begin(),resultnodes.end(),currentnode);

			if(itnode != resultnodes.end())
				resultnodes.erase(itnode);

			graph.remove(currentnode,currentedge);
			graph.remove(endnode,currentedge);
			currentnode = endnode;
		}
	}

	return true;
}

bool IdentifyMainPipes::addPathToNode(DM::Node* rootnode,std::vector<DM::Component *> nodes, std::vector<DM::Component *> edges, std::vector<DM::Component *> &resultedges, std::vector<DM::Component*> &resultnodes)
{

	if(std::find(resultnodes.begin(), resultnodes.end(),rootnode)!=resultnodes.end())
	{
		DM::Logger(DM::Error) << "Root node already connected";
		return false;
	}

	std::vector<DM::Node*> nearest = TBVectorData::findNearestNeighbours(rootnode,10000000,resultnodes);

	if(!nearest.size())
		return false;

	DM::Node *nearestnode = nearest[0];

	DynamindBoostGraph::Graph g;
	std::map<DM::Node*,int> nodesindex;
	std::map<std::pair < int, int >, DM::Edge*> nodes2edge;

	DynamindBoostGraph::createBoostGraph(nodes, edges,g,nodesindex,nodes2edge);

	std::vector<DM::Node*> pathnodes;
	std::vector<DM::Edge*> pathedges;
	std::vector < int > p(num_vertices(g));
	property_map<DynamindBoostGraph::Graph, vertex_distance_t>::type d = get(vertex_distance, g);
	double length;


	boost::dijkstra_shortest_paths( g,
									nodesindex[rootnode],
									predecessor_map(boost::make_iterator_property_map(p.begin(), get(boost::vertex_index, g)))
									.distance_map(d));

	if(!DynamindBoostGraph::findShortestPath(pathnodes,
										 pathedges,
										 length,
										 nodesindex,
										 nodes2edge,
										 d,
										 p,
										 rootnode,
										nearestnode))
		return false;

	for(int pindex=0; pindex < pathedges.size(); pindex++)
	{
		DM::Edge* currentedge = pathedges[pindex];
		DM::Node* startnode = currentedge->getStartNode();
		DM::Node* endnode = currentedge->getEndNode();

		DynamindBoostGraph::Compmap::iterator edgeit = std::find(resultedges.begin(),resultedges.end(),currentedge);

		if(edgeit==resultedges.end())
			resultedges.push_back(currentedge);

		DynamindBoostGraph::Compmap::iterator nodeit = std::find(resultnodes.begin(),resultnodes.end(),startnode);

		if(nodeit==resultnodes.end())
			resultnodes.push_back(startnode);

		nodeit = std::find(resultnodes.begin(),resultnodes.end(),endnode);

		if(nodeit==resultnodes.end())
			resultnodes.push_back(endnode);
	}

	return true;
}
