/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2011  Christian Urich

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
#include "networkanalysis.h"
#include "tbvectordata.h"

DM_DECLARE_NODE_NAME(NetworkAnalysis, Sewer)
NetworkAnalysis::NetworkAnalysis()
{

    this->network = DM::View("CONDUIT", DM::EDGE, DM::READ);
    this->network.addAttribute("Strahler");
    std::vector<DM::View> views;
    views.push_back(this->network);
    this->addData("City", views);
}

/*bool NetworkAnalysis::checkPoint(DM::Node &p, double offset) {
    foreach (Point p1, this->PointList) {
        if (p.compare2d(p1, offset))
            return true;
    }
    return false;
}
int NetworkAnalysis::findNewID(DM::Node &p, double offset) {
    int counter = 0;
    foreach (Point p1, this->PointList) {
        if (p.compare2d(p1, offset))
            return counter;
        counter++;
    }

    return -1;
}
std::vector<int> NetworkAnalysis::findConnectedEdges(int ID) {
    std::vector<int> ress;
    int counter = 0;
    foreach(Edge e, this->EdgeList) {
        if (e.getID1() == ID|| e.getID2() == ID) {
            ress.push_back(counter);
        }
        counter++;
    }

    return ress;
}*/


void NetworkAnalysis::run() {
    DM::System * city = this->getData("City");

    std::vector<std::string> names = city->getNamesOfComponentsInView(this->network);
    double offset = 10;

    std::map<DM::Node *, std::vector<DM::Edge*> > StartNodeSortedEdges;
    std::map<DM::Node *, std::vector<DM::Edge*> > EndNodeSortedEdges;
    std::map<DM::Node *, std::vector<DM::Edge*> > ConnectedEdges;
    std::vector<DM::Node *> StartNodes;
    std::vector<DM::Node *> nodes;

    //Create Connection List

    foreach(std::string name , names)  {
        DM::Edge * e = city->getEdge(name);
        DM::Node * startnode = city->getNode(e->getStartpointName());
        std::vector<DM::Edge*> v = ConnectedEdges[startnode];
        v.push_back(e);
        ConnectedEdges[startnode] = v;
        nodes.push_back(city->getNode(e->getStartpointName()));

        DM::Node * Endnode = city->getNode(e->getEndpointName());
        v = ConnectedEdges[Endnode];
        v.push_back(e);
        ConnectedEdges[Endnode] = v;
        nodes.push_back(city->getNode(e->getEndpointName()));
    }

    int counterChanged = 0;

    /*foreach (DM::Node * n, nodes) {
        //Connected Nodes
        std::vector<DM::Edge * > elements = ConnectedEdges[n];
        if (elements.size() == 0)
            continue;
        double maxDia;
        for (int i = 0; i < elements.size(); i++) {
            DM::Edge * e = elements[i];
            if (i == 0) {
                maxDia = e->getAttribute("DIAMETER")->getDouble();
                continue;
            }

            if (maxDia < e->getAttribute("DIAMETER")->getDouble())
                maxDia = e->getAttribute("DIAMETER")->getDouble();

            e->addAttribute("CHANGEDDIR", 0);
        }

        if (maxDia < 1)
            continue;

        for (int i = 0; i < elements.size(); i++) {
            DM::Edge * e = elements[i];
            double diameter = e->getAttribute("DIAMETER")->getDouble();

            if (diameter < 1 || maxDia <= diameter )
                continue;

            //Change Dir if Startnode is
            if (city->getNode(e->getStartpointName()) == n && diameter < 700) {
                std::string endnode = e->getEndpointName();
                std::string startnode = e->getStartpointName();
                e->setStartpointName(endnode);
                e->setEndpointName(startnode);
                e->addAttribute("CHANGEDDIR", 1);
                counterChanged++;
            }



        }
    }*/
    DM::Logger(DM::Debug) << "CHANGEDDIR " << counterChanged;


    //Find StartNodes
    foreach(std::string name , names)  {
        DM::Edge * e = city->getEdge(name);
        DM::Node * startnode = city->getNode(e->getStartpointName());
        std::vector<DM::Edge*> v = StartNodeSortedEdges[startnode];
        v.push_back(e);
        StartNodeSortedEdges[startnode] = v;


        DM::Node * Endnode = city->getNode(e->getEndpointName());
        v = EndNodeSortedEdges[Endnode];
        v.push_back(e);
        EndNodeSortedEdges[Endnode] = v;

    }

    foreach(std::string name , names)  {
        DM::Edge * e = city->getEdge(name);
        e->addAttribute("Strahler", 0);
        DM::Node * startnode = city->getNode(e->getStartpointName());
        DM::Node * endnode = city->getNode(e->getEndpointName());
        if (EndNodeSortedEdges[startnode].size() == 0)
            StartNodes.push_back(startnode);
        /*if (TBVectorData::getConnectedEdges(city, this->network, *endnode, 0).size() == 1) {
            StartNodes.push_back(endnode);
        }*/
    }
    DM::Logger(DM::Debug) << "Number of StartNodes" << StartNodes.size();

    foreach(DM::Node * StartID, StartNodes) {
        std::vector<DM::Edge*> ids = StartNodeSortedEdges[StartID];
        foreach(DM::Edge * id, ids) {
            id->getAttribute("Strahler")->setDouble(1);
        }
    }





    foreach(DM::Node * StartID, StartNodes) {
        std::vector<DM::Edge*> ids = StartNodeSortedEdges[StartID];
        if (ids.size()!= 1) {
            //DM::Logger(DM::Error) << "No Start Node";
            continue;
        }
        DM::Edge * e = ids[0];
        //DM::Node * n1 = city->getNode(e->getStartpointName());
        DM::Node * n2 = city->getNode(e->getEndpointName());

        DM::Node *  nextID = 0;
        //if (n1 != StartID)
        //nextID = n1;
        if (n2 != StartID)
            nextID = n2;
        if (nextID == 0) {
            DM::Logger(DM::Error) << "Something went Wrong while calculation next Point";
            continue;
        }

        int prevStrahler = 1;

        std::vector<DM::Node * > visitedNodes;
        do {
            DM::Node * nextid_tmp = 0;
            DM::Edge * outgoing_id = 0;
            visitedNodes.push_back(nextID);
            std::vector<DM::Edge*>  downstreamEdges = StartNodeSortedEdges[nextID];
            std::vector<DM::Edge*>  upstreamEdges = EndNodeSortedEdges[nextID];
            //o---o---x
            if (downstreamEdges.size() == 1) {
                DM::Edge * e = downstreamEdges[0];
                if (city->getNode(e->getEndpointName()) != nextID) {
                    nextid_tmp = city->getNode(e->getEndpointName());
                    outgoing_id = e;
                }
            }
            //Select Element with max Diameter
            if (downstreamEdges.size() > 1) {
                double maxDia = -1;
                foreach (DM::Edge * e, downstreamEdges) {
                    if (maxDia < e->getAttribute("DIAMETER")->getDouble()) {
                        maxDia =  e->getAttribute("DIAMETER")->getDouble();
                        if (city->getNode(e->getEndpointName()) != nextID) {
                            nextid_tmp = city->getNode(e->getEndpointName());
                            outgoing_id = e;
                        }
                    }
                }

            }


            if (nextid_tmp == 0 || outgoing_id == 0) {
                //DM::Logger(DM::Error) << "Unconnected Parts in Graph 2";
                break;
            }
            int maxStrahlerInNode = -1;
            int strahlerCounter = 0;
            foreach(DM::Edge * up, upstreamEdges) {
                if (e != outgoing_id) {
                    if (maxStrahlerInNode < up->getAttribute("Strahler")->getDouble()) {
                        maxStrahlerInNode = up->getAttribute("Strahler")->getDouble();
                        strahlerCounter = 1;
                    } else if (maxStrahlerInNode == up->getAttribute("Strahler")->getDouble()) {
                        strahlerCounter++;
                    }

                }
            }
            if (strahlerCounter > 1) {
                if ( outgoing_id->getAttribute("Strahler")->getDouble() < maxStrahlerInNode+1 )
                    outgoing_id->getAttribute("Strahler")->setDouble( maxStrahlerInNode+1 );

            } else {
                if (outgoing_id->getAttribute("Strahler")->getDouble() < prevStrahler) {
                    outgoing_id->getAttribute("Strahler")->setDouble( prevStrahler );
                }
            }
            prevStrahler = outgoing_id->getAttribute("Strahler")->getDouble();
            nextID = nextid_tmp;


            foreach (DM::Node * visited,visitedNodes) {
                if (nextID == visited) {
                    DM::Logger(DM::Error) << "Endless Loop";
                    nextID = 0;
                }

            }

        } while (nextID != 0);

    }
}
