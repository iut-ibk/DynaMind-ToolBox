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

#include <generalgraphanalysis.h>

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

DM_DECLARE_NODE_NAME(GeneralGraphAnalysis,Graph-Analysis)

GeneralGraphAnalysis::GeneralGraphAnalysis()
{
	this->searchdistance=200;
	//Define input parameter
	this->addParameter("Search distance for alternative paths [m]", DM::DOUBLE, &this->searchdistance);

	//Define model input
	std::vector<DM::View> views;

    //Graph
    nodesview = defhelper_graph.getView(DM::GRAPH::NODES,DM::READ);
    edgeview = defhelper_graph.getView(DM::GRAPH::EDGES,DM::READ);
    layoutedges = defhelper_graph.getView(DM::GRAPH::EDGES,DM::READ);
    layoutnodes = defhelper_graph.getView(DM::GRAPH::NODES,DM::READ);

    layoutedges.setName("LAYOUT_EDGES");
    layoutnodes.setName("LAYOUT_NODES");

    views.push_back(edgeview);
    views.push_back(nodesview);
    views.push_back(layoutedges);
    views.push_back(layoutnodes);

    this->addData("Layout", views);
}

void GeneralGraphAnalysis::run()
{
    //Clear all from previous  run
    result.clear();

	//Get System information
	this->sys = this->getData("Layout");

    DynamindBoostGraph::Compmap em = sys->getAllComponentsInView(edgeview);
    DynamindBoostGraph::Compmap nm = sys->getAllComponentsInView(nodesview);
    DynamindBoostGraph::Compmap lnm = sys->getAllComponentsInView(layoutnodes);
    DynamindBoostGraph::Compmap lem = sys->getAllComponentsInView(layoutedges);

	//Create boost graph object of original network
	DynamindBoostGraph::Graph org_g;
	std::map<DM::Node*,int> org_nodesindex;
	std::map<std::pair < int, int >, DM::Edge*> org_nodes2edge;

    DynamindBoostGraph::createBoostGraph(nm, em,org_g,org_nodesindex,org_nodes2edge);

	//Create boost graph object of possible path network
	DynamindBoostGraph::Graph g;
	std::map<DM::Node*,int> nodesindex;
	std::map<std::pair < int, int >, DM::Edge*> nodes2edge;

    DynamindBoostGraph::subtractGraphs(lem,em);
    DynamindBoostGraph::createBoostGraph(lnm, lem,g,nodesindex,nodes2edge);

    vector<DM::Node*> checknodes;
    DynamindBoostGraph::Compitr itr;
    for(itr=nm.begin(); itr!=nm.end(); ++itr)
        checknodes.push_back(static_cast<DM::Node*>((*itr).second));

    std::vector<DM::Node*> checkednodes;

    //Try to find an alternative path between junctions within one pressure zone
    uint index = 0;
    #pragma omp parallel for
    for(int source=0; source < checknodes.size(); source++)
    {
        #pragma omp critical
        {
            index++;
            checkednodes.push_back(checknodes.at(source));
        }

        if(int(index/float(checknodes.size())*100)%5==0)
            DM::Logger(DM::Standard) << index/float(checknodes.size())*100 << "%";

        //check if junction is part of an edge which is not in the current graph
        DM::Node *currentsource = checknodes.at(source);
        std::vector<DM::Edge*>  e = currentsource->getEdges();
        bool possiblenode=false;

        for(uint i = 0; i < e.size(); i++)
            if(em.find(e[i]->getUUID())==em.end())
            {
                if(lem.find(e[i]->getUUID())!=lem.end())
                    possiblenode=true;
                    break;
            }

        if(!possiblenode)
            continue;

        property_map<DynamindBoostGraph::Graph, vertex_distance_t>::type d = get(vertex_distance, g);
        property_map<DynamindBoostGraph::Graph, vertex_distance_t>::type org_d = get(vertex_distance, org_g);
        std::vector < int > p(num_vertices(g));
        std::vector < int > org_p(num_vertices(org_g));

        //calculate shortestpaths in alternative paths graph
        //Use this line if you have the possibility to compile it with VS > 2008 or all other compilers
        //boost::dijkstra_shortest_paths( g,nodesindex[junctions->at(source)],predecessor_map(&p[0]).distance_map(&d[0]));

        //boost::make_iterator_map is a bug fix for VS2008 with boost > 1.49
        boost::dijkstra_shortest_paths( g,
                                        nodesindex[checknodes.at(source)],
                                        predecessor_map(boost::make_iterator_property_map(p.begin(), get(boost::vertex_index, g)))
                                        .distance_map(d));

        //calculate shortestpaths in original paths
        //Use this line if you have the possibility to compile it with VS > 2008 or all other compilers
        //boost::dijkstra_shortest_paths( org_g,org_nodesindex[junctions->at(source)],predecessor_map(&org_p[0]).distance_map(&org_d[0]));

        //boost::make_iterator_map is a bug fix for VS2008 with boost > 1.49
        boost::dijkstra_shortest_paths( org_g,
                                        org_nodesindex[checknodes.at(source)],
                                        predecessor_map(boost::make_iterator_property_map(org_p.begin(), get(boost::vertex_index, org_g)))
                                        .distance_map(org_d));

        //find alternative paths within a circle
        std::vector<DM::Node*> nearest = TBVectorData::findNearestNeighbours(checknodes.at(source),this->searchdistance,checknodes);

        for(uint n = 0; n < nearest.size(); n++)
        {
            DM::Node *targetnode = nearest[n];
            DM::Node *rootnode = checknodes.at(source);

            std::vector<DM::Edge*>  e = targetnode->getEdges();
            bool possiblenode=false;

            for(uint i = 0; i < e.size(); i++)
                //check if rootjunction is a leaf
                if(em.find(e[i]->getUUID())==em.end())
                {
                    //check if at least one outedges exists which could be a alternative path
                    if(lem.find(e[i]->getUUID())!=lem.end())
                        possiblenode=true;
                        break;
                }

            if(!possiblenode)
                continue;

            std::vector<DM::Node*> pathnodes, org_pathnodes;
            std::vector<DM::Edge*> pathedges, org_pathedges;
            double distance, org_distance;

            if(!DynamindBoostGraph::findShortestPath(pathnodes,pathedges,distance,nodesindex,nodes2edge,d,p,rootnode,targetnode))
                continue;

            if(!DynamindBoostGraph::findShortestPath(org_pathnodes,org_pathedges,org_distance,org_nodesindex,org_nodes2edge,org_d,org_p,rootnode,targetnode))
                continue;

            bool dirty=false;
            for(uint check=1; check<pathnodes.size()-1; check++)
                if(std::find(checknodes.begin(),checknodes.end(),pathnodes[check])!=checknodes.end())
                {
                    dirty=true;
                    continue;
                }

            if(pathedges.size()==org_pathedges.size() && pathedges.size()==1)
                dirty=true;

            if(!dirty)
            {
                #pragma omp critical
                {
                    result.push_back(distance/org_distance);
                }
            }
        }
    }

    //write results
    writeResult("/tmp/testresult.txt", result);
}

void GeneralGraphAnalysis::initmodel()
{
}

double GeneralGraphAnalysis::calcPathLength(std::vector<DM::Node *> &pathnodes)
{
	double result = 0;

    for(uint index=0; index<pathnodes.size()-1; index++)
		result += TBVectorData::calculateDistance(pathnodes[index],pathnodes[index+1]);

    return result;
}

void GeneralGraphAnalysis::writeResult(string path, std::vector<double> results)
{
    QFile file(QString::fromStdString(path));
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);

    DM::Logger(DM::Standard) << "Writing " << results.size() << " results to file: " << path;
    for(uint index=0; index<results.size(); index++)
        out << QString::number(results[index]) << "\n";

    file.close();
}
