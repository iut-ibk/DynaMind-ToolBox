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

#include <minimumsteinertree.h>

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

DM_DECLARE_NODE_NAME(MinimumSteinerTree,Graph)

MinimumSteinerTree::MinimumSteinerTree()
{   
    std::vector<DM::View> views;
    DM::View view;

    view = defhelper.getCompleteView(DM::GRAPH::EDGES,DM::READ);
    views.push_back(view);
    viewdef[DM::GRAPH::EDGES]=view;

    view = defhelper.getView(DM::GRAPH::NODES,DM::READ);
    views.push_back(view);
    viewdef[DM::GRAPH::NODES]=view;

    view = DM::View("FORCEDNODES", DM::NODE, DM::READ);
    views.push_back(view);
    forcednodesview = view;

    view = defhelper.getView(DM::GRAPH::SPANNINGTREE,DM::WRITE);
    views.push_back(view);
    viewdef[DM::GRAPH::SPANNINGTREE]=view;

    this->addData("Layout", views);
}

void MinimumSteinerTree::run()
{
    DM::Logger(DM::Standard) << "Setup Graph";

    typedef adjacency_list < vecS, vecS, undirectedS, property<vertex_distance_t, int>, property < edge_weight_t, double > > Graph;
    typedef std::pair < int, int >E;

    this->sys = this->getData("Layout");
    std::vector<std::string> nodes(sys->getUUIDsOfComponentsInView(viewdef[DM::GRAPH::NODES]));
    std::vector<std::string> dedges(sys->getUUIDsOfComponentsInView(viewdef[DM::GRAPH::EDGES]));
    std::vector<std::string> forcednodes(sys->getUUIDsOfComponentsInView(forcednodesview));
    std::map<std::string,int> nodesindex;
    std::map<E,DM::Edge*> nodes2edge;
    std::list<int> forcednodeslist;

    for(uint index=0; index<nodes.size(); index++)
        nodesindex[nodes[index]]=index;

    for(uint index=0; index<forcednodes.size(); index++)
        forcednodeslist.push_back(nodesindex[forcednodes[index]]);

    const int num_nodes = nodes.size();
    Graph g(num_nodes);

    for(uint counter=0; counter<dedges.size(); counter++)
    {
        int sourceindex, targetindex;
        DM::Edge *edge=this->sys->getEdge(dedges[counter]);

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
        DM::Logger(DM::Error) << "Graph is not connected -> Forest of size: " << num;
        return;
    }

    //calculate min steiner tree approximation

    DM::Logger(DM::Standard) << "Start steiner tree algorithm with " << num_nodes << " nodes and " << dedges.size() << " edges";

    typedef std::vector<graph_traits<Graph>::vertex_descriptor> path;
    typedef adjacency_list < vecS, vecS, undirectedS, property<vertex_distance_t, int>, property < edge_weight_t, double > > SteinerGraph;

    std::map<std::pair<graph_traits<SteinerGraph>::vertex_descriptor, graph_traits<SteinerGraph>::vertex_descriptor>, path> edgetopath;
    SteinerGraph completegraph(num_vertices(g));

    //create complete graph between forced nodes
    #pragma omp parallel for
    for(uint vai = 0; vai<num_vertices(g); vai++)
    {
        if(std::find(forcednodeslist.begin(),forcednodeslist.end(),vai) == forcednodeslist.end())
            continue;

        std::vector<int> d(num_vertices(g));
        std::vector < graph_traits < Graph >::vertex_descriptor > p(num_vertices(g));
        dijkstra_shortest_paths(g,vai,predecessor_map(&p[0]).distance_map(&d[0]));
        graph_traits< Graph >::vertex_iterator vi, vend;

        for (boost::tie(vi, vend) = vertices(g); vi != vend; ++vi)
        {
            path currentpath;
            graph_traits< Graph >::vertex_descriptor currentsoure = *vi;

            if(*vi==vai)
                continue;

            if(std::find(forcednodeslist.begin(),forcednodeslist.end(),*vi) == forcednodeslist.end())
                continue;

            if(edge(vai,*vi,completegraph).second)
                continue;

            while(currentsoure!=vai)
            {
                currentpath.push_back(currentsoure);
                currentsoure=p[currentsoure];
            }

            currentpath.push_back(currentsoure);

            #pragma omp critical
            {
                edgetopath[std::make_pair<int,int>(*vi,vai)]=currentpath;
                add_edge(*vi,vai,d[*vi],completegraph);
            }
        }
    }

    //calculate minimum spanning tree of complete tree
    std::vector < graph_traits < SteinerGraph >::vertex_descriptor >p(num_vertices(completegraph));
    prim_minimum_spanning_tree(completegraph,&p[0],root_vertex(*forcednodeslist.begin()));

    //build steiner tree approximation
    for (std::size_t i = 0; i != p.size(); ++i)
    {
        if(i != p[i])
        {
            //remove_edge(i,p[i],completegraph);
            path currentpath;

            if(edgetopath.find(std::make_pair<int,int>(i,p[i]))!=edgetopath.end())
                currentpath = edgetopath[std::pair<int,int>(i,p[i])];
            else
                currentpath = edgetopath[std::pair<int,int>(p[i],i)];

            if(!currentpath.size())
                DM::Logger(DM::Debug) << "ERROR";

            for(uint index=1; index<currentpath.size(); index++)
                add_edge(currentpath[index-1],currentpath[index],0,completegraph);
        }
    }

    //map to dynamind data structure
    graph_traits< SteinerGraph >::edge_iterator ei,eend;

    for(boost::tie(ei,eend) = edges(completegraph); ei != eend; ++ei)
    {
        int s = source(*ei,completegraph);
        int t = target(*ei,completegraph);

        if(nodes2edge.find(std::make_pair<int,int>(s,t))!=nodes2edge.end())
            sys->addComponentToView(nodes2edge[std::make_pair<int,int>(s,t)],viewdef[DM::GRAPH::SPANNINGTREE]);

        if(nodes2edge.find(std::make_pair<int,int>(t,s))!=nodes2edge.end())
            sys->addComponentToView(nodes2edge[std::make_pair<int,int>(t,s)],viewdef[DM::GRAPH::SPANNINGTREE]);
    }
}

void MinimumSteinerTree::initmodel()
{
}
