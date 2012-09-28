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

#include "street2network.h"

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
#include <boosttraits.h>
#include <boost/graph/prim_minimum_spanning_tree.hpp>
#include <boost/graph/graph_concepts.hpp>

using namespace boost;

DM_DECLARE_NODE_NAME(StreetToNetwork,DynaVIBe)

StreetToNetwork::StreetToNetwork()
{   
    std::vector<DM::View> views;
    DM::View view;

    //Define Parameter street network
    view = DM::View("STREET", DM::EDGE, DM::READ);
    views.push_back(view);
    viewdef["STREET"]=view;

    //Define Parameter street network
    view = DM::View("EDGES", DM::NODE, DM::READ);
    views.push_back(view);
    viewdef["EDGES"]=view;

    //this->addData("Layout", views);
}

void StreetToNetwork::run()
{
    this->sys = this->getData("Layout");

    //TESTS

    typedef DM::System* Graph;
    BOOST_CONCEPT_ASSERT(( EdgeListGraphConcept<Graph> ));
    BOOST_CONCEPT_ASSERT(( VertexListGraphConcept<Graph> ));
    BOOST_CONCEPT_ASSERT(( ReadablePropertyMapConcept<Graph, property_traits< DM::Edge >::key_type >));
    //BOOST_CONCEPT_ASSERT(( ReadablePropertyMapConcept<Graph> ));

    //std::vector < graph_traits < DM::System* >::vertex_descriptor >p(num_vertices(this->sys));
    //property_map<Graph, edge_weight_t>::type weightmap = get(edge_weight, g);

    //prim_minimum_spanning_tree(this->sys, &p[0]);
}

void StreetToNetwork::initmodel()
{
}
