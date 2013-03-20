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

#include <extractmaxgraphofforest.h>

//DynaMind includes
#include <dmsystem.h>
#include <dmlogsink.h>

#include <iostream>
#include <fstream>
#include <QFile>
#include <QTime>
#include <map>
#include <math.h>

//BOOST GRAPH includes
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>

using namespace boost;

DM_DECLARE_NODE_NAME(ExtractMaxGraph,Graph)

ExtractMaxGraph::ExtractMaxGraph()
{   
    std::vector<DM::View> views;
    DM::View view;
    DM::GRAPH::ViewDefinitionHelper defhelper;

    view = defhelper.getView(DM::GRAPH::EDGES,DM::MODIFY);
    views.push_back(view);
    viewdef[DM::GRAPH::EDGES]=view;

    view = defhelper.getView(DM::GRAPH::NODES,DM::MODIFY);
    views.push_back(view);
    viewdef[DM::GRAPH::NODES]=view;

    this->addData("Layout", views);
}

void ExtractMaxGraph::run()
{
    DM::Logger(DM::Standard) << "Setup Graph";

    typedef adjacency_list < vecS, vecS, undirectedS, property<vertex_distance_t, int>, property < edge_weight_t, int > > Graph;
    typedef std::pair < int, int >E;

    this->sys = this->getData("Layout");
    std::vector<std::string> nodes(sys->getUUIDsOfComponentsInView(viewdef[DM::GRAPH::NODES]));
    std::vector<std::string> edges(sys->getUUIDsOfComponentsInView(viewdef[DM::GRAPH::EDGES]));
    std::map<std::string,int> nodesindex;
    std::map<E,DM::Edge*> nodes2edge;

    for(uint index=0; index<nodes.size(); index++)
        nodesindex[nodes[index]]=index;

    std::vector<E> edgeindex(edges.size());

    const int num_nodes = nodes.size();
    Graph g(num_nodes);

    for(uint counter=0; counter<edges.size(); counter++)
    {
        int sourceindex, targetindex;
        DM::Edge *edge=this->sys->getEdge(edges[counter]);

        sourceindex=nodesindex[edge->getStartpointName()];
        targetindex=nodesindex[edge->getEndpointName()];

        edgeindex[counter]=E(sourceindex,targetindex);
        nodes2edge[E(sourceindex,targetindex)]=edge;
        add_edge(sourceindex, targetindex, 1, g);
    }

    //check if graph is conntected
    std::vector<int> component(num_vertices(g));
    int num = connected_components(g, &component[0]);

    DM::Logger(DM::Standard) << "Number of graphs found: " << num;

    std::map<int,int> componentsizes;

    for (uint i = 0; i != component.size(); ++i)
        componentsizes[component[i]]=++componentsizes[component[i]];

    int maxgraphindex=0;

    for(uint index=0; index < componentsizes.size(); index++)
    {
        if(componentsizes[maxgraphindex] < componentsizes[index])
            maxgraphindex = index;

        DM::Logger(DM::Standard) << "Graph " << (int)index+1 << " has " << componentsizes[index] << " elements";
    }

    DM::Logger(DM::Standard) << "Graph " << maxgraphindex+1 << " is extracted";

    //remove edges from forest of graphs
    for(uint index=0; index < edges.size(); index++)
    {
        E current = edgeindex[index];

        if(component[current.first]!=maxgraphindex)
        {
            DM::Edge *realedge = nodes2edge[current];
            this->sys->removeComponentFromView(realedge,viewdef[DM::GRAPH::EDGES]);
            this->sys->removeComponentFromView(realedge->getStartNode(),viewdef[DM::GRAPH::NODES]);
            this->sys->removeComponentFromView(realedge->getEndNode(),viewdef[DM::GRAPH::NODES]);
        }
    }

    //remove edges from forest of graphs
    for(uint index=0; index < nodes.size(); index++)
    {
        std::string nodeid = nodes[index];

        if(component[nodesindex[nodeid]]!=maxgraphindex)
            this->sys->removeComponentFromView(sys->getComponent(nodeid),viewdef[DM::GRAPH::NODES]);
    }
}

void ExtractMaxGraph::initmodel()
{
}
