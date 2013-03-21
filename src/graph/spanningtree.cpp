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
    this->algtest=false;

    this->addParameter("Prim minimum spanning tree", DM::BOOL, &this->algprim);
    this->addParameter("Random spanning tree", DM::BOOL, &this->algrand);
    this->addParameter("Test algorithm not for productive use", DM::BOOL, &this->algtest);

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

    typedef std::pair < int, int >E;

    this->sys = this->getData("Layout");
    DynamindBoostGraph::Compmap nodes = sys->getAllComponentsInView(viewdef[DM::GRAPH::NODES]);
    DynamindBoostGraph::Compmap edges = sys->getAllComponentsInView(viewdef[DM::GRAPH::EDGES]);
    std::map<DM::Node*,int> nodesindex;
    std::map<std::pair<int,int> ,DM::Edge*> nodes2edge;

    boost::mt19937 rng(time(NULL));

    DynamindBoostGraph::Graph g;

    DynamindBoostGraph::createBoostGraph(nodes,edges,g,nodesindex,nodes2edge);

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
    std::vector < graph_traits < DynamindBoostGraph::Graph >::vertex_descriptor >p(num_vertices(g));

    if(this->algrand)
    {
        this->algprim = false;
        this->algtest = false;
        DM::Logger(DM::Standard) << "Start random spanning tree algorithm with " << nodes.size() << " nodes and " << edges.size() << " edges";
        random_spanning_tree(g, rng, root_vertex(*vertices(g).first).vertex_index_map(get(vertex_index,g)).predecessor_map(&p[0]).weight_map(get(edge_weight,g)));
    }

    if(this->algtest)
    {
        DM::Logger(DM::Warning) << "DO NOT USE THIS ALGORITHM IF YOU DO NOT KNOW WHAT IT IS DOING --- IT IS ONLY FOR TESTING";
        DM::Logger(DM::Standard) << "Start test algorithm algorithm with " << nodes.size() << " nodes and " << edges.size() << " edges";

        testalg(g);
        this->algprim = true;
        //END TESTING
    }

    if(this->algprim)
    {
        DM::Logger(DM::Standard) << "Start prim minimum spanning tree algorithm with " << nodes.size() << " nodes and " << edges.size() << " edges";
        prim_minimum_spanning_tree(g, &p[0]);
    }

    //clean view
    for(DynamindBoostGraph::Compitr itr = nodes.begin(); itr != nodes.end(); ++itr)
        sys->removeComponentFromView((*itr).second,viewdef[DM::GRAPH::NODES]);

    for(DynamindBoostGraph::Compitr itr = edges.begin(); itr != edges.end(); ++itr)
        sys->removeComponentFromView((*itr).second,viewdef[DM::GRAPH::EDGES]);

    DM::Logger(DM::Standard) << "Starting extracting results from algorithm";

    //extract spanning tree
    vector< DM::Component* > insertednodes;

    for (std::size_t i = 1; i < p.size(); i++)
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

void SpanningTree::testalg(DynamindBoostGraph::Graph &g)
{
    std::vector < graph_traits < DynamindBoostGraph::Graph >::vertex_descriptor >p(num_vertices(g));
    typedef std::map< graph_traits < DynamindBoostGraph::Graph >::edge_descriptor , double> Prob;
    typedef Prob::iterator Probitr;

    Prob probability;
    boost::mt19937 rng(time(NULL));

    for(uint iteration = 0; iteration < 1000; iteration++)
    {
        random_spanning_tree(g, rng, root_vertex(*vertices(g).first).vertex_index_map(get(vertex_index,g)).predecessor_map(&p[0]).weight_map(get(edge_weight,g)));

        for (std::size_t i = 1; i < p.size(); i++)
        {
            if(i != p[i])
            {
                graph_traits < DynamindBoostGraph::Graph >::edge_descriptor e;
                bool found;

                boost::tie(e, found) = edge(p[i],i, g);

                if(!found)
                    boost::tie(e, found) = edge(i,p[i], g);

                if(!found)
                    continue;

                probability[e]=probability[e]+1;
            }
        }
    }

    //update weights
    for(Probitr i = probability.begin(); i != probability.end(); ++i)
        if((*i).second != 0.0)
            put(edge_weight,g,(*i).first,get(edge_weight,g,(*i).first)/(*i).second);
}

