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

#ifndef SpanningTree_H
#define SpanningTree_H

#include <dmmodule.h>
#include <dm.h>
#include <graphviewdef.h>
#include <dynamindboostgraphhelper.h>
#include <boost/graph/prim_minimum_spanning_tree.hpp>
#include <boost/graph/kruskal_min_spanning_tree.hpp>
#include <boost/graph/random_spanning_tree.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/connected_components.hpp>
#include <boost/random/mersenne_twister.hpp>

using namespace boost;
class SpanningTree : public DM::Module
{
    DM_DECLARE_NODE(SpanningTree)

private:
    typedef std::map<DM::GRAPH::COMPONENTS,DM::View> viewmap;
    typedef std::pair < int, int >E;

    DM::System *sys;
    viewmap viewdef;
    DM::GRAPH::ViewDefinitionHelper defhelper;
    bool algprim, algrand, algtest, algkruskal;

public:
    SpanningTree();
    typedef std::map<std::string,std::string> stringmap;
    typedef std::map<std::string, stringmap * > conversionmap;

    void run();
    void initmodel();
    void insertByPredecessors(std::vector < graph_traits < DynamindBoostGraph::Graph >::vertex_descriptor > &p,
                              std::map<DM::Node*,int> &nodesindex,
                              std::map<std::pair<int,int> ,DM::Edge*> &nodes2edge,
                              std::vector<int> &component,
                              int &maxgraphindex);
    void insertByEdges(std::vector < DynamindBoostGraph::Graph::edge_descriptor > &spanning_tree,
                       DynamindBoostGraph::Graph &g,
                       std::map<std::pair<int,int> ,DM::Edge*> &nodes2edge);

private:
    void testalg(DynamindBoostGraph::Graph &g);
};

#endif // SpanningTree_H
