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

#include <connectnodes2graph.h>

//DynaMind includes
#include <dmsystem.h>
#include <dmlogsink.h>
#include <math.h>

DM_DECLARE_NODE_NAME(ConnectNodes2Graph,Graph)

ConnectNodes2Graph::ConnectNodes2Graph()
{   
    std::vector<DM::View> views;
    DM::View view;

    //Define Parameter street network
    view = DM::View("EDGES", DM::EDGE, DM::WRITE);
    views.push_back(view);
    viewdef["EDGES"]=view;

    //Define Parameter street network
    view = DM::View("NODES", DM::NODE, DM::WRITE);
    views.push_back(view);
    viewdef["NODES"]=view;

    //Nodes which should be connected to the graph
    view = DM::View("CONNECTINGNODES", DM::NODE, DM::READ);
    views.push_back(view);
    viewdef["CONNECTINGNODES"]=view;

    this->addData("Layout", views);
}

void ConnectNodes2Graph::run()
{
    this->sys = this->getData("Layout");
    std::vector<std::string> nodes(sys->getUUIDsOfComponentsInView(viewdef["NODES"]));
    std::vector<std::string> connectingnodes(sys->getUUIDsOfComponentsInView(viewdef["CONNECTINGNODES"]));

    if(!nodes.size())
    {
        DM::Logger(DM::Error) << "Graph does not contain any nodes -> EMPTY GRAPH";
        return;
    }

    if(!connectingnodes.size())
    {
        DM::Logger(DM::Error) << "No connecting nodes are existing";
        return;
    }

    //connectnodes
    //TODO slow version
    for(int index=0; index < connectingnodes.size(); index++)
    {
        DM::Node *connectingnode = sys->getNode(connectingnodes[index]);
        std::string nearest = findNearestNode(nodes,connectingnode);
        sys->addEdge(connectingnode,sys->getNode(nearest),viewdef["EDGES"]);
        sys->addComponentToView(connectingnode,viewdef["NODES"]);
    }
}

std::string ConnectNodes2Graph::findNearestNode(std::vector<std::string>& nodes, DM::Node *connectingNode)
{
    double currentdistance=calcDistance(sys->getNode(nodes[0]),connectingNode);
    DM::Node *node = sys->getNode(nodes[0]);

    for(int index=1; index<nodes.size(); index++)
    {
        double distance = calcDistance(sys->getNode(nodes[index]),connectingNode);

        if(currentdistance > distance)
        {
            currentdistance=distance;
            node=sys->getNode(nodes[index]);
        }
    }

    return node->getUUID();
}

double ConnectNodes2Graph::calcDistance(DM::Node *a, DM::Node *b)
{
    return sqrt(pow(a->getX()-b->getX(),2)+pow(a->getY()-b->getY(),2));
}
