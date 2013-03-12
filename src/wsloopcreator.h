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

#ifndef LoopCreator_H
#define LoopCreator_H

#include <dmmodule.h>
#include <dm.h>

#include <graphviewdef.h>
#include <watersupplyviewdef.h>

#include <boost/graph/adjacency_list.hpp>

#define NULLPOINT -100

class LoopCreator : public DM::Module
{
    DM_DECLARE_NODE(LoopCreator)

private:
    DM::System *sys;
    DM::GRAPH::ViewDefinitionHelper defhelper_graph;
    DM::WS::ViewDefinitionHelper defhelper_ws;

public:
    LoopCreator();

    void run();
    void initmodel();

private:
    uint getZone(double elevation, double zonesize);
    bool createBoostGraph(std::map<std::string,DM::Component*> &nodes,
                          std::map<std::string,DM::Component*> &edges,
                          boost::adjacency_list < boost::vecS, boost::vecS, boost::undirectedS, boost::property<boost::vertex_distance_t, int>, boost::property < boost::edge_weight_t, double > > &Graph,
                          std::map<DM::Node*,int> &nodesindex,
                          std::map<std::pair < int, int >, DM::Edge*> &nodes2edge);
};

#endif // LoopCreator_H
