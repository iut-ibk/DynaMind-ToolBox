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

#include <totalgraphweight.h>

//DynaMind includes
#include <dmsystem.h>
#include <dmlogsink.h>

DM_DECLARE_NODE_NAME(TotalGraphWeight,Graph-Analysis)

TotalGraphWeight::TotalGraphWeight()
{
	//Define model input
	std::vector<DM::View> views;

    //Graph
    nodesview = defhelper_graph.getView(DM::GRAPH::NODES,DM::READ);
    edgeview = defhelper_graph.getView(DM::GRAPH::EDGES,DM::READ);
    defhelper_graph.setAttribute(DM::GRAPH::EDGES,DM::GRAPH::EDGES_ATTR_DEF::Weight,edgeview,DM::READ);

    views.push_back(edgeview);
    views.push_back(nodesview);

    this->addData("Graph", views);
}

void TotalGraphWeight::run()
{
    double result = 0.0;

    typedef std::map<std::string, DM::Component*> CM;
    typedef CM::iterator CMItr;

	//Get System information
    this->sys = this->getData("Graph");

    CM em = sys->getAllComponentsInView(edgeview);
    CM nm = sys->getAllComponentsInView(nodesview);

    CMItr itr;
    for(itr=em.begin(); itr!=em.end(); ++itr)
    {
        DM::Edge *currentedge = static_cast<DM::Edge*>((*itr).second);
        result+=currentedge->getAttribute(defhelper_graph.getAttributeString(DM::GRAPH::EDGES,DM::GRAPH::EDGES_ATTR_DEF::Weight))->getDouble();
    }

    DM::Logger(DM::Standard) << "Number of nodes: " << nm.size();
    DM::Logger(DM::Standard) << "Number of edges: " << em.size();
    DM::Logger(DM::Standard) << "Total graph weight: " << result;

    return;
}
