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
#include <boost/graph/prim_minimum_spanning_tree.hpp>
#include <boost/graph/random_spanning_tree.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <time.h>

//viewdefs
#include <examinationroomviewdef.h>

using namespace boost;

DM_DECLARE_NODE_NAME(LoopCreator,Watersupply)

LoopCreator::LoopCreator()
{    
    DM::ER::ViewDefinitionHelper defhelper_er;
    std::vector<DM::View> views;
    DM::View view;

    //Possible edges and nodes
    view = defhelper_graph.getView(DM::GRAPH::EDGES,DM::READ);
    views.push_back(view);

    view = defhelper_graph.getView(DM::GRAPH::NODES,DM::READ);
    views.push_back(view);

    //examination room
    view = defhelper_er.getView(DM::ER::EXAMINATIONROOM,DM::READ);
    views.push_back(view);

    //ws system
    view = defhelper_ws.getView(DM::WS::JUNCTION,DM::MODIFY);
    views.push_back(view);

    view = defhelper_ws.getView(DM::WS::PIPE,DM::MODIFY);
    views.push_back(view);

    this->addData("Layout", views);
}

void LoopCreator::run()
{
    /*
    DM::Logger(DM::Standard) << "Setup Graph";

    typedef adjacency_list < vecS, vecS, undirectedS, property<vertex_distance_t, int>, property < edge_weight_t, double > > Graph;
    typedef std::pair < int, int >E;

    this->sys = this->getData("Layout");
    std::vector<std::string> nodes(sys->getUUIDsOfComponentsInView(defhelper_graph.getCompleteView(DM::GRAPH::EDGES,DM::READ)));
    std::vector<std::string> edges(sys->getUUIDsOfComponentsInView(defhelper_graph.getCompleteView(DM::GRAPH::NODES,DM::READ)));
    std::map<std::string,int> nodesindex;
    std::map<E,DM::Edge*> nodes2edge;
    boost::mt19937 rng(time(NULL));
    */
}

void LoopCreator::initmodel()
{
}
