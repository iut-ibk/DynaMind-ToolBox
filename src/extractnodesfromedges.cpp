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

DM_DECLARE_NODE_NAME(ExtractNodesFromEdges,DynaVIBe)

ExtractNodesFromEdges::ExtractNodesFromEdges()
{   
    std::vector<DM::View> views;
    DM::View view;

    //Define Parameter street network
    view = DM::View("EDGES", DM::EDGE, DM::READ);
    views.push_back(view);
    viewdef["EDGES"]=view;

    //Define Parameter street network
    view = DM::View("NODES", DM::NODE, DM::WRITE);
    views.push_back(view);
    viewdef["NODES"]=view;

    this->addData("Layout", views);
}

void ExtractNodesFromEdges::run()
{
    this->sys = this->getData("Layout");
    std::vector<std::string> edges(sys->getUUIDsOfComponentsInView(viewdef["EDGES"]));
    std::map<std::string,bool> nodesadded;
    //std::vector<std::string> nodesadded;

    DM::Logger(DM::Standard) << "Number of Edges found:" << edges.size();

    for(int index=0; index<edges.size(); index++)
    {
        DM::Edge *edge = this->sys->getEdge(edges.at(index));
        string sname = edge->getStartpointName();
        string tname = edge->getEndpointName();

        //SOURCE
        if(nodesadded.find(sname)==nodesadded.end())
        {
            this->sys->addComponentToView(this->sys->getNode(sname),viewdef["NODES"]);
            nodesadded[sname]=true;
        }


        //TARGET
        if(nodesadded.find(tname)==nodesadded.end())
        {
            this->sys->addComponentToView(this->sys->getNode(tname),viewdef["NODES"]);
            nodesadded[tname]=true;
        }

        //if((index%200)==0)
        //    DM::Logger(DM::Standard) << index << " of " << edges.size();
    }
}
