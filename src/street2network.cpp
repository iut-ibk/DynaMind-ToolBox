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

#include <street2network.h>

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
//#include <boosttraits.h>
#include <boost/graph/prim_minimum_spanning_tree.hpp>
#include <boost/graph/adjacency_list.hpp>

using namespace boost;

DM_DECLARE_NODE_NAME(StreetToNetwork,DynaVIBe)

StreetToNetwork::StreetToNetwork()
{   
    std::vector<DM::View> views;
    DM::View view;

    //Define Parameter street network
    view = DM::View("EDGES", DM::EDGE, DM::READ);
    views.push_back(view);
    viewdef["EDGES"]=view;

    //Define Parameter street network
    view = DM::View("NODES", DM::NODE, DM::READ);
    views.push_back(view);
    viewdef["NODES"]=view;

    view = DM::View("SPANNINGTREE", DM::EDGE, DM::WRITE);
    views.push_back(view);
    viewdef["SPANNINGTREE"]=view;


    this->addData("Layout", views);
}

void StreetToNetwork::run()
{
    DM::Logger(DM::Standard) << "Setup Graph";

    typedef adjacency_list < vecS, vecS, undirectedS, property<vertex_distance_t, int>, property < edge_weight_t, int > > Graph;
    typedef std::pair < int, int >E;

    this->sys = this->getData("Layout");
    std::vector<std::string> nodes(sys->getUUIDsOfComponentsInView(viewdef["NODES"]));
    std::vector<std::string> edges(sys->getUUIDsOfComponentsInView(viewdef["EDGES"]));
    std::map<std::string,int> nodesindex;
    std::map<E,DM::Edge*> nodes2edge;

    for(int index=0; index<nodes.size(); index++)
        nodesindex[nodes[index]]=index;

    E edgeindex[edges.size()];
    int weights[edges.size()];

    const int num_nodes = nodes.size();

    for(int counter=0; counter<edges.size(); counter++)
    {
        int sourceindex, targetindex;
        DM::Edge *edge=this->sys->getEdge(edges[counter]);

        sourceindex=nodesindex[edge->getStartpointName()];
        targetindex=nodesindex[edge->getEndpointName()];

        edgeindex[counter]=E(sourceindex,targetindex);
        nodes2edge[edgeindex[counter]]=edge;
        weights[counter] = counter;
    }

    Graph g(edgeindex, edgeindex + sizeof(edgeindex) / sizeof(E), weights, num_nodes);
    std::vector < graph_traits < Graph >::vertex_descriptor >p(num_vertices(g));

    DM::Logger(DM::Standard) << "Start prim algorithm with " << num_nodes << " nodes and " << edges.size() << " edges";

    prim_minimum_spanning_tree(g, &p[0]);

    bool dirtygraph=0;

    for (std::size_t i = 0; i != p.size(); ++i)
    {
        if(i != p[i])
        {
            DM::Edge *currentnode = nodes2edge[E(p[i],i)];

            if(!currentnode)
                currentnode = nodes2edge[E(i,p[i])];

            this->sys->addComponentToView(currentnode,viewdef["SPANNINGTREE"]);
        }
        else
        {
            dirtygraph++;
        }
    }

    DM::Logger(DM::Standard) << "Edges containt in spanning tree: " << sys->getUUIDsOfComponentsInView(viewdef["SPANNINGTREE"]).size();

    if(dirtygraph > 1)
        DM::Logger(DM::Warning) << "Graph is not connected";
}

void StreetToNetwork::initmodel()
{
}
