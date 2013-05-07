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

#include <wsloopcreator.h>

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

//viewdefs
#include <examinationroomviewdef.h>

using namespace boost;

DM_DECLARE_NODE_NAME(LoopCreator,Watersupply)

LoopCreator::LoopCreator()
{    
    this->searchdistance=200;
    this->minloopdiameter=250;
    this->maxnumberelements=3;
    this->zonesize = 60;
    //Define input parameter
    this->addParameter("Search distance for alternative paths [m]", DM::DOUBLE, &this->searchdistance);
    this->addParameter("Minimum diameter of loop [m]", DM::DOUBLE, &this->minloopdiameter);
    this->addParameter("Maximum number of additional elements to create one loop [-]", DM::DOUBLE, &this->maxnumberelements);
    this->addParameter("Pressure within one zone [m]", DM::DOUBLE, &this->zonesize);

    //Define model input
    DM::ER::ViewDefinitionHelper defhelper_er;
    std::vector<DM::View> views;

    //Possible edges and nodes
    views.push_back(defhelper_graph.getView(DM::GRAPH::EDGES,DM::READ));
    views.push_back(defhelper_graph.getView(DM::GRAPH::NODES,DM::READ));

    //examination room
    //views.push_back(defhelper_er.getView(DM::ER::EXAMINATIONROOM,DM::READ));

    //ws system
    views.push_back(defhelper_ws.getView(DM::WS::JUNCTION,DM::MODIFY));
    views.push_back(defhelper_ws.getView(DM::WS::PIPE,DM::MODIFY));

    this->addData("Layout", views);
}

void LoopCreator::run()
{
    //Define vars
    //DM::ER::ViewDefinitionHelper defhelper_er;
    PressureZones zones;

    //Get System information
    this->sys = this->getData("Layout");
    DynamindBoostGraph::Compmap em = sys->getAllComponentsInView(defhelper_graph.getView(DM::GRAPH::EDGES,DM::READ));
    DynamindBoostGraph::Compmap nm = sys->getAllComponentsInView(defhelper_graph.getView(DM::GRAPH::NODES,DM::READ));
    DynamindBoostGraph::Compmap jm = sys->getAllComponentsInView(defhelper_ws.getView(DM::WS::JUNCTION,DM::MODIFY));
    DynamindBoostGraph::Compmap pm = sys->getAllComponentsInView(defhelper_ws.getView(DM::WS::PIPE,DM::MODIFY));
    //Compmap erm = sys->getAllComponentsInView(defhelper_er.getView(DM::ER::EXAMINATIONROOM,DM::READ));

    DM::Logger(DM::Standard) << "Calculate pressure zone boundaries of water supply system";
    double minelevation, maxelevation, meanelevation;
    calcPressureZonesBoundaries(jm,minelevation,maxelevation, meanelevation);

    DM::Logger(DM::Standard) << "Calculate pressure zones of water supply system";
    calculatePressureZones(jm, zones,zonesize,meanelevation);

    DM::Logger(DM::Standard) << "Remove all edges which are crossing two pressure zones from graph";
    removeCrossingZoneEdges(em,zonesize,meanelevation);

    DM::Logger(DM::Standard) << "Remove all edges which are already pipes from graph";
    DynamindBoostGraph::subtractGraphs(em,pm);

    //Create boost graph object of original network
    DynamindBoostGraph::Graph org_g;
    std::map<DM::Node*,int> org_nodesindex;
    std::map<std::pair < int, int >, DM::Edge*> org_nodes2edge;

    DynamindBoostGraph::createBoostGraph(jm, pm,org_g,org_nodesindex,org_nodes2edge);

    //Create boost graph object of possible path network
    DynamindBoostGraph::Graph g;
    std::map<DM::Node*,int> nodesindex;
    std::map<std::pair < int, int >, DM::Edge*> nodes2edge;

    DynamindBoostGraph::createBoostGraph(nm, em,g,nodesindex,nodes2edge);

    //starting algorithm for finding alternative paths
    DM::Logger(DM::Standard) << "System has " << zones.size() << " pressure zones";
    DM::Logger(DM::Standard) << "Searching for alternative paths within pressure zones";

    int pzone = 1;

    for(PressureZones::iterator itr = zones.begin(); itr!= zones.end(); itr++)
    {
        DM::Logger(DM::Standard) << "Finding alternative paths within pressure zone number " << pzone++ << "and label " << (int)((*itr).first);
        std::vector<DM::Component*> addedcomponents;
        //uint currentzone = (*itr).first;
        boost::shared_ptr< std::vector< DM::Node* > > junctions = (*itr).second;

        //Try to find an alternative path between junctions within one pressure zone
        #pragma omp parallel for
        for(int source=0; source < junctions->size(); source++)
        {
#ifdef _OPENMP
			//DM::Logger(DM::Standard) << "zone " << pzone << " | source " << source << "/" << junctions->size()
			//	<< " | thread " << omp_get_thread_num() << "/" << omp_get_num_threads() << "/" << omp_get_max_threads();
#endif
            property_map<DynamindBoostGraph::Graph, vertex_distance_t>::type d = get(vertex_distance, g);
            property_map<DynamindBoostGraph::Graph, vertex_distance_t>::type org_d = get(vertex_distance, org_g);
            std::vector < int > p(num_vertices(g));
            std::vector < int > org_p(num_vertices(org_g));

            //calculate shortestpaths in alternative paths graph
            //Use this line if you have the possibility to compile it with VS > 2008 or all other compilers
            //boost::dijkstra_shortest_paths( g,nodesindex[junctions->at(source)],predecessor_map(&p[0]).distance_map(&d[0]));

            //boost::make_iterator_map is a bug fix for VS2008 with boost > 1.49
            boost::dijkstra_shortest_paths( g,
                                            nodesindex[junctions->at(source)],
                                            predecessor_map(boost::make_iterator_property_map(p.begin(), get(boost::vertex_index, g)))
                                            .distance_map(d));

            //calculate shortestpaths in original paths
            //Use this line if you have the possibility to compile it with VS > 2008 or all other compilers
            //boost::dijkstra_shortest_paths( org_g,org_nodesindex[junctions->at(source)],predecessor_map(&org_p[0]).distance_map(&org_d[0]));

            //boost::make_iterator_map is a bug fix for VS2008 with boost > 1.49
            boost::dijkstra_shortest_paths( org_g,
                                            org_nodesindex[junctions->at(source)],
                                            predecessor_map(boost::make_iterator_property_map(org_p.begin(), get(boost::vertex_index, org_g)))
                                            .distance_map(org_d));

            //find alternative paths within a circle
            std::vector<DM::Node*> nearest = TBVectorData::findNearestNeighbours(junctions->at(source),this->searchdistance,*junctions);

            for(uint n = 0; n < nearest.size(); n++)
            {
                DM::Node *targetjunction = nearest[n];
                DM::Node *rootjunction = junctions->at(source);

                std::vector<DM::Node*> pathnodes, org_pathnodes;
                std::vector<DM::Edge*> pathedges, org_pathedges;
                double distance, org_distance;

                if(!DynamindBoostGraph::findShortestPath(pathnodes,pathedges,distance,nodesindex,nodes2edge,d,p,rootjunction,targetjunction))
                    continue;

                if(!DynamindBoostGraph::findShortestPath(org_pathnodes,org_pathedges,org_distance,org_nodesindex,org_nodes2edge,org_d,org_p,rootjunction,targetjunction))
                    continue;

                if(pathedges.size()>maxnumberelements)
                    continue;

                double maxdistance = TBVectorData::maxDistance(pathnodes,pathnodes[pathnodes.size()-1]);
                double org_maxdistance = TBVectorData::maxDistance(org_pathnodes,org_pathnodes[org_pathnodes.size()-1]);

                if(maxdistance < minloopdiameter/2 && org_maxdistance < minloopdiameter/2)
                    continue;
				#pragma omp critical
                addPathToSystem(pathnodes, pathedges, addedcomponents);
            }

        }
    }
}

void LoopCreator::initmodel()
{
}

uint LoopCreator::getZone(double elevation, double zonesize, double mean)
{
    double NULLPOINT = (int)(mean-(zonesize/2))%((int)zonesize);
    return (uint)((elevation - NULLPOINT)/zonesize);
}

void LoopCreator::calculatePressureZones(DynamindBoostGraph::Compmap &nodes, LoopCreator::PressureZones &zones, double zonesize, double mean)
{
    for(DynamindBoostGraph::Compitr itr = nodes.begin(); itr != nodes.end(); itr++)
    {
        DM::Node* currentnode = static_cast<DM::Node*>((*itr).second);
        uint currentzone = getZone(currentnode->getZ(),zonesize,mean);

        if(zones.find(currentzone)==zones.end())
            zones[currentzone]=boost::make_shared< std::vector< DM::Node* > >(std::vector< DM::Node* >());

        zones[currentzone]->push_back(currentnode);
    }
}

void LoopCreator::removeCrossingZoneEdges(DynamindBoostGraph::Compmap &edges, double zonesize, double mean)
{
    for(DynamindBoostGraph::Compitr itr = edges.begin(); itr != edges.end(); ++itr)
    {
        DM::Edge* currentedge = static_cast<DM::Edge*>((*itr).second);

        if(getZone(currentedge->getStartNode()->getZ(),zonesize,mean) != getZone(currentedge->getEndNode()->getZ(),zonesize,mean))
            edges.erase(currentedge->getUUID());
    }
}

void LoopCreator::addPathToSystem(std::vector<DM::Node *>& pathnodes, std::vector<DM::Edge *>& pathedges, std::vector<DM::Component *> &addedcomponents)
{
    for(uint index=0; index < pathnodes.size(); index++)
    {
        if(find(addedcomponents.begin(), addedcomponents.end(), pathnodes[index]) == addedcomponents.end())
        {
            sys->addComponentToView(pathnodes[index],defhelper_ws.getView(DM::WS::JUNCTION,DM::MODIFY));
			addedcomponents.push_back(pathnodes[index]);
        }
    }

    for(uint index=0; index < pathedges.size(); index++)
    {
        if(find(addedcomponents.begin(), addedcomponents.end(), pathedges[index]) == addedcomponents.end())
        {
            sys->addComponentToView(pathedges[index],defhelper_ws.getView(DM::WS::PIPE,DM::MODIFY));
            addedcomponents.push_back(pathedges[index]);
        }
    }
}

void LoopCreator::calcPressureZonesBoundaries(DynamindBoostGraph::Compmap &nodes, double &min, double &max, double &mean)
{
    std::vector<double> elevations;
    min = 0;
    max = 0;
    mean = 0;

    if(!nodes.size())
        return;

    for(DynamindBoostGraph::Compitr itr = nodes.begin(); itr != nodes.end(); ++itr)
        elevations.push_back(static_cast<DM::Node*>((*itr).second)->getZ());

    min = *std::min_element(elevations.begin(), elevations.end());
    max = *std::max_element(elevations.begin(), elevations.end());
    mean = std::accumulate(elevations.begin(), elevations.end(),0.0)/elevations.size();

    return;
}
