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

//BOOST GRAPH includes
//#include <boosttraits.h>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/random_spanning_tree.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <time.h>
#include <boost/make_shared.hpp>

//viewdefs
#include <examinationroomviewdef.h>

using namespace boost;

DM_DECLARE_NODE_NAME(LoopCreator,Watersupply)

LoopCreator::LoopCreator()
{    
    this->searchdistance=200;
    this->maxelements=5;
    this->minlengthrelation=3;
    //Define input parameter
    this->addParameter("Search distance for alternative paths [m]", DM::DOUBLE, &this->searchdistance);
    this->addParameter("Max number of elements within an alternative path [-]", DM::DOUBLE, &this->maxelements);
    this->addParameter("Min quotient between alternative path and original path [-]", DM::DOUBLE, &this->minlengthrelation);

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
    double zonesize = 10;

    //Get System information
    this->sys = this->getData("Layout");
    Compmap em = sys->getAllComponentsInView(defhelper_graph.getView(DM::GRAPH::EDGES,DM::READ));
    Compmap nm = sys->getAllComponentsInView(defhelper_graph.getView(DM::GRAPH::NODES,DM::READ));
    Compmap jm = sys->getAllComponentsInView(defhelper_ws.getView(DM::WS::JUNCTION,DM::MODIFY));
    Compmap pm = sys->getAllComponentsInView(defhelper_ws.getView(DM::WS::PIPE,DM::MODIFY));
    //Compmap erm = sys->getAllComponentsInView(defhelper_er.getView(DM::ER::EXAMINATIONROOM,DM::READ));

    DM::Logger(DM::Standard) << "Calculate pressure zones of water supply system";
    calculatePressureZones(jm, zones,zonesize);

    DM::Logger(DM::Standard) << "Remove all edges which are crossing two pressure zones from graph";
    removeCrossingZoneEdges(em,zonesize);

    DM::Logger(DM::Standard) << "Remove all edges which are already pipes from graph";
    subtractGraphs(em,pm);

    //Create boost graph object of original network
    Graph org_g;
    std::map<DM::Node*,int> org_nodesindex;
    std::map<std::pair < int, int >, DM::Edge*> org_nodes2edge;

    createBoostGraph(jm, pm,org_g,org_nodesindex,org_nodes2edge);

    //Create boost graph object of possible path network
    Graph g;
    std::map<DM::Node*,int> nodesindex;
    std::map<std::pair < int, int >, DM::Edge*> nodes2edge;

    createBoostGraph(nm, em,g,nodesindex,nodes2edge);

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
        for(uint source=0; source < junctions->size(); source++)
        {
            property_map<Graph, vertex_distance_t>::type d = get(vertex_distance, g);
            property_map<Graph, vertex_distance_t>::type org_d = get(vertex_distance, org_g);
            std::vector < graph_traits < Graph >::vertex_descriptor > p(num_vertices(g));
            std::vector < graph_traits < Graph >::vertex_descriptor > org_p(num_vertices(org_g));

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
            std::vector<DM::Node*> nearest = findNearestNeighbours(junctions->at(source),this->searchdistance,*junctions);

            for(uint n = 0; n < nearest.size(); n++)
            {
                DM::Node *targetjunction = nearest[n];
                DM::Node *rootjunction = junctions->at(source);

                std::vector<DM::Node*> pathnodes, org_pathnodes;
                std::vector<DM::Edge*> pathedges, org_pathedges;
                double distance, org_distance;

                if(!findShortestPath(pathnodes,pathedges,distance,nodesindex,nodes2edge,d,p,rootjunction,targetjunction))
                    continue;

                if(!findShortestPath(org_pathnodes,org_pathedges,org_distance,org_nodesindex,org_nodes2edge,org_d,org_p,rootjunction,targetjunction))
                    continue;

                if(pathedges.size()>maxelements)
                    continue;

                double lpath = max<double>(distance,org_distance);
                double spath = min<double>(distance,org_distance);

                if(lpath/spath <= this->minlengthrelation)
                    continue;

                addPathToSystem(pathnodes, pathedges, addedcomponents);
            }

        }
    }
}

void LoopCreator::initmodel()
{
}

uint LoopCreator::getZone(double elevation, double zonesize)
{
    return (uint)((elevation - NULLPOINT)/zonesize);
}

bool LoopCreator::createBoostGraph(std::map<std::string,DM::Component*> &nodes,
                                   std::map<std::string,DM::Component*> &edges,
                                   Graph &g,
                                   std::map<DM::Node*,int> &nodesindex,
                                   std::map<std::pair < int, int >, DM::Edge*> &nodes2edge)
{
    typedef std::map<std::string,DM::Component*>::iterator Compitr;

    g.clear();
    int nodeindex=0;
    for(Compitr itr = nodes.begin(); itr!=nodes.end(); itr++)
    {
        nodesindex[static_cast<DM::Node*>((*itr).second)]=nodeindex;
        add_vertex(nodeindex,g);
        nodeindex++;
    }


    for(Compitr itr = edges.begin(); itr!=edges.end(); itr++)
    {
        int sourceindex, targetindex;
        DM::Edge *edge = static_cast<DM::Edge*>((*itr).second);

        double distance = edge->getAttribute(defhelper_graph.getAttributeString(DM::GRAPH::EDGES,DM::GRAPH::EDGES_ATTR_DEF::Weight))->getDouble();

        sourceindex = nodesindex[edge->getStartNode()];
        targetindex = nodesindex[edge->getEndNode()];

        nodes2edge[std::make_pair(sourceindex,targetindex)]=edge;
        add_edge(sourceindex, targetindex, distance, g);
    }

    return true;
}

void LoopCreator::calculatePressureZones(LoopCreator::Compmap &nodes, LoopCreator::PressureZones &zones, double zonesize)
{
    for(Compitr itr = nodes.begin(); itr != nodes.end(); itr++)
    {
        DM::Node* currentnode = static_cast<DM::Node*>((*itr).second);
        uint currentzone = getZone(currentnode->getZ(),zonesize);

        if(zones.find(currentzone)==zones.end())
            zones[currentzone]=boost::make_shared< std::vector< DM::Node* > >(std::vector< DM::Node* >());

        zones[currentzone]->push_back(currentnode);
    }
}

void LoopCreator::removeCrossingZoneEdges(LoopCreator::Compmap &edges, double zonesize)
{
    for(Compitr itr = edges.begin(); itr != edges.end(); ++itr)
    {
        DM::Edge* currentedge = static_cast<DM::Edge*>((*itr).second);

        if(getZone(currentedge->getStartNode()->getZ(),zonesize) != getZone(currentedge->getEndNode()->getZ(),zonesize))
            edges.erase(currentedge->getUUID());
    }
}

void LoopCreator::subtractGraphs(LoopCreator::Compmap &a, LoopCreator::Compmap &b)
{
    for(Compitr itr = b.begin(); itr != b.end(); ++itr)
    {
        DM::Edge* currentedge = static_cast<DM::Edge*>((*itr).second);
        a.erase(currentedge->getUUID());
    }
}

bool LoopCreator::findShortestPath(std::vector<DM::Node*> &pathnodes,
                                   std::vector<DM::Edge*> &pathedges,
                                   double &distance,
                                   std::map<DM::Node*,int> &nodesindex,
                                   std::map<std::pair < int, int >, DM::Edge*> &nodes2edge,
                                   property_map<Graph, vertex_distance_t>::type &distancevector,
                                   std::vector<long unsigned int> &predecessors,
                                   DM::Node* root, DM::Node* target)
{
    DM::Node *targetjunction = target;
    DM::Node *rootjunction = root;
    int tindex = nodesindex[targetjunction];
    int rindex = nodesindex[rootjunction];
    pathnodes.push_back(target);
    std::vector<int> path;
    path.push_back(tindex);

    if(targetjunction==rootjunction)
        return false;

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

        if(nodes2edge.find(std::make_pair(sindex,tindex))!=nodes2edge.end())
            newpipe = nodes2edge[std::make_pair(sindex,tindex)];

        if(nodes2edge.find(std::make_pair(tindex,sindex))!=nodes2edge.end())
            newpipe = nodes2edge[std::make_pair(tindex,sindex)];

        if(!newpipe)
        {
            DM::Logger(DM::Warning) << "Could not find specific pipe in system (ERROR ?)";
            return false;
        }

        DM::Node *targetjunction = newpipe->getEndNode();
        DM::Node *sourcejunction = newpipe->getStartNode();

        if(sourcejunction != newpipe->getStartNode())
        {
            sourcejunction = newpipe->getEndNode();
            targetjunction = newpipe->getStartNode();
        }

        pathnodes.push_back(sourcejunction);
        pathnodes.push_back(targetjunction);
        pathedges.push_back(newpipe);
    }

    distance = distancevector[path[0]];
    return true;
}

void LoopCreator::addPathToSystem(std::vector<DM::Node *> pathnodes, std::vector<DM::Edge *> pathedges, std::vector<DM::Component *> &addedcomponents)
{
    #pragma omp critical
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
}

std::vector<DM::Node*> LoopCreator::findNearestNeighbours(DM::Node *root, double maxdistance, std::vector<DM::Node *> nodefield)
{
    std::vector<DM::Node*> result;
    result.push_back(root);

    for(uint i=0; i < nodefield.size(); i++)
    {
        double currentdistance=TBVectorData::calculateDistance(root,nodefield[i]);
        if(currentdistance <= maxdistance)
            result.push_back(nodefield[i]);
    }
    return result;
}
