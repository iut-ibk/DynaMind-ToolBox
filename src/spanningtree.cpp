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

#include <spanningtree.h>

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
#include <boost/graph/random_spanning_tree.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <time.h>

using namespace boost;

DM_DECLARE_NODE_NAME(SpanningTree,Graph)

SpanningTree::SpanningTree()
{    
    this->algprim=true;
    this->algrand=false;
    this->addParameter("Prim minimum spanning tree", DM::BOOL, &this->algprim);
    this->addParameter("Random spanning tree", DM::BOOL, &this->algrand);

    std::vector<DM::View> views;
    DM::View view;
    DM::GRAPH::ViewDefinitionHelper defhelper;

    view = defhelper.getCompleteView(DM::GRAPH::EDGES,DM::MODIFY);
    views.push_back(view);
    viewdef[DM::GRAPH::EDGES]=view;

    view = defhelper.getView(DM::GRAPH::NODES,DM::MODIFY);
    views.push_back(view);
    viewdef[DM::GRAPH::NODES]=view;

    this->addData("Layout", views);
}

void SpanningTree::run()
{
    DM::Logger(DM::Standard) << "Setup Graph";

    typedef adjacency_list < vecS, vecS, undirectedS, property<vertex_distance_t, double>, property < edge_weight_t, double > > Graph;
    typedef std::pair < int, int >E;

    this->sys = this->getData("Layout");
    std::vector<std::string> nodes(sys->getUUIDsOfComponentsInView(viewdef[DM::GRAPH::NODES]));
    std::vector<std::string> edges(sys->getUUIDsOfComponentsInView(viewdef[DM::GRAPH::EDGES]));
    std::map<std::string,int> nodesindex;
    std::map<E,DM::Edge*> nodes2edge;
    boost::mt19937 rng(time(NULL));

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

        DM::Logger(DM::Standard) << "Graph " << (int)index+1 << " has " << componentsizes[index] << " elements";
    }

    if(num!=1)
    {
        DM::Logger(DM::Warning) << "Graph is not connected -> Forest of size: " << num;
        DM::Logger(DM::Warning) << "PLEASE USE MODULE NAMED EXTRACTMAXGRAPH TO CLEAN THE FOREST";
        return;
    }

    //calculate spanning tree or forest of graphs
    std::vector < graph_traits < Graph >::vertex_descriptor >p(num_vertices(g));

    if(this->algprim)
    {
        DM::Logger(DM::Standard) << "Start prim minimum spanning tree algorithm with " << num_nodes << " nodes and " << edges.size() << " edges";
        prim_minimum_spanning_tree(g, &p[0]);
    }

    if(this->algrand)
    {
        DM::Logger(DM::Standard) << "Start random spanning tree algorithm with " << num_nodes << " nodes and " << edges.size() << " edges";
        random_spanning_tree(g, rng, root_vertex(*vertices(g).first).vertex_index_map(get(vertex_index,g)).predecessor_map(&p[0]).weight_map(get(edge_weight,g)));
    }

    //clean view
    for(uint index = 0; index < nodes.size(); index++)
        sys->removeComponentFromView(sys->getComponent(nodes[index]),viewdef[DM::GRAPH::NODES]);

    for(uint index = 0; index < edges.size(); index++)
        sys->removeComponentFromView(sys->getComponent(edges[index]),viewdef[DM::GRAPH::EDGES]);

    DM::Logger(DM::Standard) << "Starting extracting results from algorithm";

    //extract spanning tree
    vector< DM::Component* > insertednodes;

    for (std::size_t i = 0; i < p.size(); i++
         )
    {
        if(i != p[i])
        {
            if(component[i]!=maxgraphindex)
                continue;

            DM::Edge* edge = 0;
            DM::Node* start = 0;
            DM::Node* end = 0;

            if(nodes2edge.find(E(p[i],i))!=nodes2edge.end())
                edge = static_cast<DM::Edge*>(nodes2edge[E(p[i],i)]);

            if(nodes2edge.find(E(i,p[i]))!=nodes2edge.end())
                edge = static_cast<DM::Edge*>(nodes2edge[E(i,p[i])]);

            if(!edge)
            {
                DM::Logger(DM::Error) << "Could not find specific edge which should exist";
                continue;
            }

            this->sys->addComponentToView(edge,viewdef[DM::GRAPH::EDGES]);

            start = edge->getStartNode();
            end = edge->getEndNode();

            if(find(insertednodes.begin(),insertednodes.end(),start)==insertednodes.end())
                this->sys->addComponentToView(start,viewdef[DM::GRAPH::NODES]);

            if(find(insertednodes.begin(),insertednodes.end(),end)==insertednodes.end())
                this->sys->addComponentToView(end,viewdef[DM::GRAPH::NODES]);
        }
    }

    DM::Logger(DM::Standard) << "Edges containt in spanning tree: " << sys->getAllComponentsInView(viewdef[DM::GRAPH::EDGES]).size();
    DM::Logger(DM::Standard) << "Number of created trees: " << num;
}

void SpanningTree::initmodel()
{
}
