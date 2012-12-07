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

#include <minimumspanningtree.h>

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

//BOOST GRAPH includes
//#include <boosttraits.h>
#include <boost/graph/prim_minimum_spanning_tree.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>

using namespace boost;

DM_DECLARE_NODE_NAME(MinimumSpanningTree,Graph)

MinimumSpanningTree::MinimumSpanningTree()
{   
    std::vector<DM::View> views;
    DM::View view;

    view = defhelper.getCompleteView(DM::GRAPH::EDGES,DM::READ);
    views.push_back(view);
    viewdef[DM::GRAPH::EDGES]=view;

    view = defhelper.getView(DM::GRAPH::NODES,DM::READ);
    views.push_back(view);
    viewdef[DM::GRAPH::NODES]=view;

    view = defhelper.getView(DM::GRAPH::SPANNINGTREE,DM::WRITE);
    views.push_back(view);
    viewdef[DM::GRAPH::SPANNINGTREE]=view;

    this->addData("Layout", views);
}

void MinimumSpanningTree::run()
{
    DM::Logger(DM::Standard) << "Setup Graph";

    typedef adjacency_list < vecS, vecS, undirectedS, property<vertex_distance_t, int>, property < edge_weight_t, double > > Graph;
    typedef std::pair < int, int >E;

    this->sys = this->getData("Layout");
    std::vector<std::string> nodes(sys->getUUIDsOfComponentsInView(viewdef[DM::GRAPH::NODES]));
    std::vector<std::string> edges(sys->getUUIDsOfComponentsInView(viewdef[DM::GRAPH::EDGES]));
    std::map<std::string,int> nodesindex;
    std::map<E,DM::Edge*> nodes2edge;

    for(uint index=0; index<nodes.size(); index++)
        nodesindex[nodes[index]]=index;

    const int num_nodes = nodes.size();
    Graph g(num_nodes);

    for(uint counter=0; counter<edges.size(); counter++)
    {
        int sourceindex, targetindex;
        DM::Edge *edge=this->sys->getEdge(edges[counter]);

        double distance = edge->getAttribute(defhelper.getAttributeString(DM::GRAPH::EDGES,DM::GRAPH::EDGES_ATTR_DEF::Weight))->getDouble();

        sourceindex=nodesindex[edge->getStartpointName()];
        targetindex=nodesindex[edge->getEndpointName()];

        nodes2edge[E(sourceindex,targetindex)]=edge;
        add_edge(sourceindex, targetindex, distance, g);
    }

    //check if graph is conntected
    std::vector<int> component(num_vertices(g));
    int num = connected_components(g, &component[0]);

    std::map<int,int> componentsizes;

    for (uint i = 0; i != component.size(); ++i)
        componentsizes[component[i]]=++componentsizes[component[i]];

    int maxgraphindex=0;

    for(uint index=0; index < componentsizes.size(); index++)
    {
        if(componentsizes[maxgraphindex] < componentsizes[index])
            maxgraphindex = index;

        DM::Logger(DM::Standard) << "Tree " << (int)index+1 << " has " << componentsizes[index] << " elements";
    }

    if(num!=1)
    {
        DM::Logger(DM::Warning) << "Graph is not connected -> Forest of size: " << num;
        DM::Logger(DM::Warning) << "Graph " << maxgraphindex+1 << " is used for building a minimum spanning tree";
    }

    //calculate min spanning tree or forest of minimum spanning trees
    std::vector < graph_traits < Graph >::vertex_descriptor >p(num_vertices(g));
    DM::Logger(DM::Standard) << "Start prim algorithm with " << num_nodes << " nodes and " << edges.size() << " edges";

    prim_minimum_spanning_tree(g, &p[0]);

    for (std::size_t i = 0; i != p.size(); ++i)
    {
        if(i != p[i])
        {
            if(component[i]!=maxgraphindex)
                continue;

            if(nodes2edge.find(E(p[i],i))!=nodes2edge.end())
            {
                this->sys->addComponentToView(nodes2edge[E(p[i],i)],viewdef[DM::GRAPH::SPANNINGTREE]);
                continue;
            }

            if(nodes2edge.find(E(i,p[i]))!=nodes2edge.end())
            {
                this->sys->addComponentToView(nodes2edge[E(i,p[i])],viewdef[DM::GRAPH::SPANNINGTREE]);
                continue;
            }
        }
    }

    DM::Logger(DM::Standard) << "Edges containt in spanning tree: " << sys->getUUIDsOfComponentsInView(viewdef[DM::GRAPH::SPANNINGTREE]).size();
    DM::Logger(DM::Standard) << "Number of created trees: " << num;
}

void MinimumSpanningTree::initmodel()
{
}
