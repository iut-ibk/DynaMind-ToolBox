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

#ifndef DynaMindBoostGraphHelper_H
#define DynaMindBoostGraphHelper_H

#include <dm.h>
#include <boost/graph/adjacency_list.hpp>
#include <string.h>
#include <vector>

class DM_HELPER_DLL_LOCAL DynamnindBoostGraph
{
public:
    //Typedef
    typedef std::map<std::string, DM::Component*> Compmap;
    typedef Compmap::iterator Compitr;
    typedef boost::adjacency_list < boost::vecS,
                                    boost::vecS,
                                    boost::undirectedS,
                                    boost::property<boost::vertex_distance_t, double>,
                                    boost::property < boost::edge_weight_t, double > > Graph;

    //Functions
    static std::vector<DM::Node*> findNearestNeighbours(DM::Node* root, double maxdistance, std::vector<DM::Node*> nodefield);
    static bool findShortestPath(std::vector<DM::Node*> &pathnodes,
                          std::vector<DM::Edge*> &pathedges,
                          double &distance,
                          std::map<DM::Node*,int> &nodesindex,
                          std::map<std::pair < int, int >, DM::Edge*> &nodes2edge,
                          boost::property_map<Graph, boost::vertex_distance_t>::type &distancevector,
                          std::vector<long unsigned int> &predecessors,
                          DM::Node* root, DM::Node* target);
    static void subtractGraphs(Compmap &a, Compmap &b); // result = a-b
    static bool createBoostGraph(std::map<std::string,DM::Component*> &nodes,
                          std::map<std::string,DM::Component*> &edges,
                          Graph &Graph,
                          std::map<DM::Node*,int> &nodesindex,
                          std::map<std::pair < int, int >, DM::Edge*> &nodes2edge);
};

#endif // DynaMindBoostGraphHelper_H
