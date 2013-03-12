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
    typedef std::map<std::string, DM::Component*> Compmap;
    typedef std::map<uint, boost::shared_ptr< std::vector< DM::Node* > > > PressureZones;
    typedef Compmap::iterator Compitr;

    //Define vars
    DM::ER::ViewDefinitionHelper defhelper_er;
    PressureZones zones, graph_zones;
    double zonesize = 10;

    //Get System information
    this->sys = this->getData("Layout");
    Compmap em = sys->getAllComponentsInView(defhelper_graph.getView(DM::GRAPH::EDGES,DM::READ));
    Compmap nm = sys->getAllComponentsInView(defhelper_graph.getView(DM::GRAPH::NODES,DM::READ));
    Compmap jm = sys->getAllComponentsInView(defhelper_ws.getView(DM::WS::JUNCTION,DM::MODIFY));
    Compmap pm = sys->getAllComponentsInView(defhelper_ws.getView(DM::WS::PIPE,DM::MODIFY));
    //Compmap erm = sys->getAllComponentsInView(defhelper_er.getView(DM::ER::EXAMINATIONROOM,DM::READ));

    DM::Logger(DM::Standard) << "Calculate pressure zones of water supply system";
    for(Compitr itr = jm.begin(); itr != jm.end(); itr++)
    {
        DM::Node* currentnode = static_cast<DM::Node*>((*itr).second);
        uint currentzone = getZone(currentnode->getZ(),zonesize);

        if(zones.find(currentzone)==zones.end())
            zones[currentzone]=boost::make_shared< std::vector< DM::Node* > >(std::vector< DM::Node* >());

        zones[currentzone]->push_back(currentnode);
    }

    /*
    DM::Logger(DM::Standard) << "Calculate possible pressure zones of additional nodes";
    for(Compitr itr = nm.begin(); itr != nm.end(); itr++)
    {
        DM::Node* currentnode = static_cast<DM::Node*>((*itr).second);
        uint currentzone = getZone(currentnode->getZ(),zonesize);

        if(graph_zones.find(currentzone)==graph_zones.end())
            graph_zones[currentzone]=boost::make_shared< std::vector< DM::Node* > >(std::vector< DM::Node* >());

        graph_zones[currentzone]->push_back(currentnode);
    }
    */


    DM::Logger(DM::Debug) << "Graph has " << em.size() << " edges";
    DM::Logger(DM::Standard) << "Remove all edges which are element of a dead end path";
    for(Compitr itr = nm.begin(); itr != nm.end(); ++itr)
    {
        DM::Node* currentnode = static_cast<DM::Node*>((*itr).second);

        while(currentnode->getEdges().size()==1)
        {
            DM::Edge* currentedge = currentnode->getEdges()[0];

            if(currentedge->getStartNode()==currentnode)
                currentnode=currentedge->getEndNode();
            else
                currentnode=currentedge->getStartNode();

            em.erase(currentedge->getUUID());
        }
    }

    DM::Logger(DM::Debug) << "Graph has " << em.size() << " edges";
    DM::Logger(DM::Standard) << "Remove all edges which are crossing two pressure zones from graph";
    for(Compitr itr = em.begin(); itr != em.end(); ++itr)
    {
        DM::Edge* currentedge = static_cast<DM::Edge*>((*itr).second);

        if(getZone(currentedge->getStartNode()->getZ(),NULLPOINT) != getZone(currentedge->getEndNode()->getZ(),NULLPOINT))
            em.erase(currentedge->getUUID());
    }

    DM::Logger(DM::Debug) << "Graph has " << em.size() << " edges";
    DM::Logger(DM::Standard) << "Remove all edges which are already pipes from graph";
    for(Compitr itr = pm.begin(); itr != pm.end(); ++itr)
    {
        DM::Edge* currentedge = static_cast<DM::Edge*>((*itr).second);
        em.erase(currentedge->getUUID());
    }
    DM::Logger(DM::Debug) << "Graph has " << em.size() << " edges";

    //Create boost graph object
    typedef boost::adjacency_list < vecS, vecS, undirectedS, property<vertex_distance_t, int>, property < edge_weight_t, double > > Graph;
    Graph g;
    std::map<DM::Node*,int> nodesindex;
    std::map<std::pair < int, int >, DM::Edge*> nodes2edge;

    createBoostGraph(nm, em,g,nodesindex,nodes2edge);

    DM::Logger(DM::Standard) << "System has " << zones.size() << " pressure zones";
    DM::Logger(DM::Standard) << "Searching for alternative paths within pressure zones";

    for(PressureZones::iterator itr = zones.begin(); itr!= zones.end(); itr++)
    {
        //uint currentzone = (*itr).first;
        boost::shared_ptr< std::vector< DM::Node* > > junctions = (*itr).second;

        //Try to find an alternative path between junctions within one pressure zone
        for(uint source=0; source < junctions->size(); source++)
        {
            std::vector<int> d(num_vertices(g));
            std::vector < graph_traits < Graph >::vertex_descriptor > p(num_vertices(g));

            //Use this line if you have the possibility to compile it with VS > 2008 or all other compilers
            //boost::dijkstra_shortest_paths( g,nodesindex[junctions->at(source)],predecessor_map(&p[0]).distance_map(&d[0]));

            //boost::make_iterator_map is a bug fix for VS2008 with boost > 1.49
            boost::dijkstra_shortest_paths( g,
                                            nodesindex[junctions->at(source)],
                                            predecessor_map(boost::make_iterator_property_map(p.begin(), get(boost::vertex_index, g)))
                                            .distance_map(boost::make_iterator_property_map(d.begin(), get(boost::vertex_index, g))));
            graph_traits< Graph >::vertex_iterator vi, vend;

            for(uint n = 0; n < junctions->size(); n++)
            {
                DM::Node *sourcejunction = junctions->at(n);

                while(true)
                {
                    int sindex = nodesindex[sourcejunction];
                    int tindex = p[sindex];
                    DM::Edge *newpipe = 0;

                    if(sindex==tindex)
                        break;

                    if(nodes2edge.find(std::make_pair(sindex,tindex))!=nodes2edge.end())
                        newpipe = nodes2edge[std::make_pair(sindex,tindex)];

                    if(nodes2edge.find(std::make_pair(tindex,sindex))!=nodes2edge.end())
                        newpipe = nodes2edge[std::make_pair(tindex,sindex)];

                    if(!newpipe)
                    {
                        DM::Logger(DM::Warning) << "Could not find specific pipe in system (ERROR ?)";
                        continue;
                    }

                    DM::Node *targetjunction = newpipe->getEndNode();

                    if(sourcejunction != newpipe->getStartNode())
                    {
                        sourcejunction = newpipe->getEndNode();
                        targetjunction = newpipe->getStartNode();
                    }

                    Compmap tmppm = sys->getAllComponentsInView(defhelper_ws.getView(DM::WS::PIPE,DM::MODIFY));

                    if(tmppm.find(newpipe->getUUID())==tmppm.end())
                        sys->addComponentToView(newpipe,defhelper_ws.getView(DM::WS::PIPE,DM::MODIFY));

                    Compmap tmpjm = sys->getAllComponentsInView(defhelper_ws.getView(DM::WS::JUNCTION,DM::MODIFY));

                    if(tmpjm.find(sourcejunction->getUUID())==tmpjm.end())
                        sys->addComponentToView(sourcejunction,defhelper_ws.getView(DM::WS::JUNCTION,DM::MODIFY));

                    if(tmpjm.find(targetjunction->getUUID())==tmpjm.end())
                        sys->addComponentToView(targetjunction,defhelper_ws.getView(DM::WS::JUNCTION,DM::MODIFY));

                    if(std::find(junctions->begin(),junctions->end(),targetjunction)!=junctions->end())
                        break;
                }
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
                                   boost::adjacency_list < vecS, vecS, undirectedS, property<vertex_distance_t, int>, property < edge_weight_t, double > > &g,
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
