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

#include <extractnodesfromedges.h>

//DynaMind includes
#include <dmsystem.h>
#include <dmlogsink.h>

DM_DECLARE_NODE_NAME(ExtractNodesFromEdges,Graph)

ExtractNodesFromEdges::ExtractNodesFromEdges()
{   
    std::vector<DM::View> views;
    DM::View view;
    DM::GRAPH::ViewDefinitionHelper defhelper;

    //Define Parameter street network
    view = defhelper.getView(DM::GRAPH::EDGES,DM::READ);
    views.push_back(view);
    viewdef[DM::GRAPH::EDGES]=view;

    //Define Parameter street network
    view = defhelper.getView(DM::GRAPH::NODES,DM::WRITE);
    views.push_back(view);
    viewdef[DM::GRAPH::NODES]=view;

    this->addData("Layout", views);
}

void ExtractNodesFromEdges::run()
{
    this->sys = this->getData("Layout");

    std::vector<std::string> edges(sys->getUUIDsOfComponentsInView(viewdef[DM::GRAPH::EDGES]));
    std::map<std::string,bool> nodesadded;

    DM::Logger(DM::Standard) << "Number of Edges found:" << edges.size();

    for(uint index=0; index<edges.size(); index++)
    {
        DM::Edge *edge = this->sys->getEdge(edges.at(index));
        string sname = edge->getStartpointName();
        string tname = edge->getEndpointName();

        //SOURCE
        if(nodesadded.find(sname)==nodesadded.end())
        {
            this->sys->addComponentToView(this->sys->getNode(sname),viewdef[DM::GRAPH::NODES]);
            nodesadded[sname]=true;
        }

        //TARGET
        if(nodesadded.find(tname)==nodesadded.end())
        {
            this->sys->addComponentToView(this->sys->getNode(tname),viewdef[DM::GRAPH::NODES]);
            nodesadded[tname]=true;
        }   
    }

    DM::Logger(DM::Standard) << "Number of extracted nodes: " << nodesadded.size();
}
