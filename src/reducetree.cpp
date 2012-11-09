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

#include <reducetree.h>

//DynaMind includes
#include <dmsystem.h>
#include <dmlogsink.h>
#include <math.h>

DM_DECLARE_NODE_NAME(ReduceTree,Graph)

ReduceTree::ReduceTree()
{   
    std::vector<DM::View> views;
    DM::View view;

    view = DM::View("SPANNINGTREE", DM::EDGE, DM::READ);
    views.push_back(view);
    viewdef["SPANNINGTREE"]=view;

    view = DM::View("REDUCEDTREE", DM::EDGE, DM::WRITE);
    views.push_back(view);
    viewdef["REDUCEDTREE"]=view;

    //Nodes which should be connected to the graph
    view = DM::View("FORCEDNODES", DM::NODE, DM::READ);
    views.push_back(view);
    viewdef["FORCEDNODES"]=view;

    this->addData("Layout", views);
}

void ReduceTree::run()
{
    this->sys = this->getData("Layout");
    std::vector<std::string> edges(sys->getUUIDsOfComponentsInView(viewdef["SPANNINGTREE"]));
    std::vector<std::string> forcednodes(sys->getUUIDsOfComponentsInView(viewdef["FORCEDNODES"]));
    std::vector<std::string> deleteedges;

    if(!edges.size())
    {
        DM::Logger(DM::Error) << "Cannot reduce an emply tree";
        return;
    }

    DM::Logger(DM::Standard) << "Find delete nodes";

    //reduce
    deleteflagiterative(edges,forcednodes,deleteedges);

    //delete nodes
    DM::Logger(DM::Standard) << "Delete nodes";
    int delcounter = 0;
    for(uint edgeindex=0; edgeindex<edges.size(); edgeindex++)
    {
        std::string currentedge = edges[edgeindex];
        DM::Edge *edge = sys->getEdge(currentedge);

        if(std::find(deleteedges.begin(), deleteedges.end(),currentedge)!=deleteedges.end())
            delcounter++;
        else
            sys->addComponentToView(edge,viewdef["REDUCEDTREE"]);
    }

    DM::Logger(DM::Standard) << "Removed " << delcounter << " edges.";
    return;
}

//iterative
void ReduceTree::deleteflagiterative(std::vector<std::string> &edges, std::vector<std::string> &forcednodes, std::vector<std::string> &deleteedges)
{
    std::map<std::string,int> edgecount;
    std::list<std::string> noforceddeadends;
    std::multimap<std::string,DM::Edge* > node2edges;
    std::multimap<std::string,DM::Edge*>::iterator ite;

    for(int edgeindex=0; edgeindex < edges.size(); edgeindex++)
    {
        std::string currentedge = edges[edgeindex];
        DM::Edge *edge = sys->getEdge(currentedge);

        edgecount[edge->getStartpointName()] += 1;
        edgecount[edge->getEndpointName()] += 1;

        node2edges.insert(std::pair<std::string,DM::Edge*>(edge->getStartpointName(),edge));
        node2edges.insert(std::pair<std::string,DM::Edge*>(edge->getEndpointName(),edge));
    }

    std::map<std::string,int>::const_iterator it;

    for(it = edgecount.begin();it != edgecount.end();++it)
    {
        std::string name = (*it).first;
        int value = (*it).second;

        if(std::find(forcednodes.begin(),forcednodes.end(),name)==forcednodes.end())
            if(value==1)
                noforceddeadends.push_back(name);
    }

    while( noforceddeadends.size() )
    {

        std::string currentnode = noforceddeadends.front();

        for(ite=node2edges.equal_range(currentnode).first; ite!=node2edges.equal_range(currentnode).second; ++ite)
        {
            //std::string currentedge = edges[edgeindex];
            DM::Edge *edge = (*ite).second;

            if(std::find(deleteedges.begin(), deleteedges.end(),edge->getUUID())==deleteedges.end())
            {
                std::string newnode = "";

                if(!edge->getStartpointName().compare(currentnode))
                    newnode = edge->getEndpointName();
                else
                    newnode = edge->getStartpointName();

                edgecount[currentnode] -= 1;
                noforceddeadends.erase(std::find(noforceddeadends.begin(),noforceddeadends.end(),currentnode));

                deleteedges.push_back(edge->getUUID());
                edgecount.erase(currentnode);

                currentnode=newnode;
                edgecount[currentnode] -= 1;

                if(std::find(forcednodes.begin(),forcednodes.end(),currentnode)==forcednodes.end())
                {
                    if(edgecount[currentnode]==1)
                        noforceddeadends.push_back(currentnode);

                    if(edgecount[currentnode]==0)
                        noforceddeadends.erase(std::find(noforceddeadends.begin(),noforceddeadends.end(),currentnode));
                }

                break;
            }
        }
    }
}
