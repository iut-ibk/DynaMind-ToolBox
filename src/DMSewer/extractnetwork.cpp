/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of VIBe2
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
#include "extractnetwork.h"
#include <sstream>
#include "csg_s_operations.h"
#include "tbvectordata.h"

DM_DECLARE_NODE_NAME(ExtractNetwork, Sewer)
void ExtractNetwork::AgentExtraxtor::run() {

    this->path.clear();
    for (int i = 0; i < this->steps; i++) {
        this->path.push_back(currentPos);

        double currentH = this->Topology->getValue(currentPos.x, currentPos.y);
        double hcurrent = currentPos.h;
        //Influence Topology
        ConnectivityField->getMoorNeighbourhood(neigh, currentPos.x,currentPos.y);
        int index = GenerateSewerNetwork::indexOfMinValue(neigh);
        for (int i = 0; i < 9; i++) {

            if (index == i) {
                decissionVector[i] = 1;
            } else {
                decissionVector[i] = 0;
            }
        }

        //Forbidden Areas
        ForbiddenAreas->getMoorNeighbourhood(neigh, currentPos.x,currentPos.y);
        for (int i = 0; i < 9; i++) {
            decissionVector[i]*=neigh[i];
        }


        if (index == -1) {
            this->alive = false;
            break;
        }

        int direction = index;
        this->currentPos.x+=csg_s::csg_s_operations::returnPositionX(direction);
        this->currentPos.y+=csg_s::csg_s_operations::returnPositionY(direction);

        double deltaH = currentH - this->Topology->getValue(currentPos.x, currentPos.y);

        if (deltaH > 0) {
            currentPos.h = hcurrent - deltaH;
            if (currentPos.h < 3) {
                currentPos.h = 3;
            }
        } else {
            currentPos.h=hcurrent-deltaH;
        }

        if (currentPos.x < 0 || currentPos.y < 0 || currentPos.x > MarkPath->getWidth()-2 || currentPos.y >  MarkPath->getHeight()-2) {
            this->alive = false;
            break;


        }
        if (Goals->getValue(currentPos.x, currentPos.y ) > 0 || this->MarkPath->getValue(currentPos.x, currentPos.y) > 0) {
            if (currentPos.h < 3.1) {
                this->alive = false;
                this->successful = true;
                this->path.push_back(currentPos);
                break;
            }
        }


    }

    this->alive = false;
}

ExtractNetwork::ExtractNetwork()
{
    this->steps = 1000;
    this->ConduitLength = 200;
    /*this->IdentifierConduit = "Conduit_";
    this->IdentifierEnd = "EndPoint_";
    this->IdentifierInlet = "Inlet_";
    this->IdentifierShaft = "Shaft_";*/
    this->addParameter("Steps", DM::LONG, & this->steps);

    this->addParameter("ConduitLength", DM::DOUBLE, &this->ConduitLength);

    confield = DM::View("ConnectivityField_in", DM::RASTERDATA, DM::READ);
    path =DM::View("Path", DM::RASTERDATA, DM::READ);
    forb = DM::View("ForbiddenAreas", DM::RASTERDATA, DM::READ);
    goals = DM::View("Goals", DM::RASTERDATA, DM::READ);

    std::vector<DM::View> sewerGen;
    sewerGen.push_back(confield);
    sewerGen.push_back(path);
    sewerGen.push_back(forb);
    sewerGen.push_back(goals);
    this->addData("sewerGeneration", sewerGen);

    std::vector<DM::View> city;
    topo = DM::View("Topology", DM::RASTERDATA, DM::READ);
    Conduits = DM::View("CONDUIT", DM::EDGE, DM::MODIFY);
    Conduits.modifyAttribute("New");
    Inlets= DM::View("INLET",  DM::NODE, DM::READ);
    Inlets.modifyAttribute("New");
    Inlets.modifyAttribute("Used");
    Junction= DM::View("JUNCTION",  DM::NODE, DM::MODIFY);
    Junction.modifyAttribute("D");
    Junction.modifyAttribute("Z");
    EndPoint = DM::View("WWTP", DM::NODE, DM::READ);

    city.push_back(topo);
    city.push_back(Conduits);
    city.push_back(Inlets);
    city.push_back(Junction);
    city.push_back(EndPoint);

    this->addData("City", city);

}
void ExtractNetwork::run() {

    this->city = this->getData("City");


    this->ConnectivityField = this->getRasterData("sewerGeneration", confield);
    this->Goals = this->getRasterData("sewerGeneration", goals);
    this->Path =  this->getRasterData("sewerGeneration", path);
    this->ForbiddenAreas = this->getRasterData("sewerGeneration", forb);
    this->Topology = this->getRasterData("City", topo);

    long width = this->ConnectivityField->getWidth();
    long height = this->ConnectivityField->getHeight();
    double cellSize = this->ConnectivityField->getCellSize();

    this->Path->setSize(width, height, cellSize);
    this->Path->clear();

    std::vector<AgentExtraxtor * > agents;

    std::vector<DM::Node*> StartPos;
    foreach (std::string inlet, city->getNamesOfComponentsInView(Inlets))  {
        DM::Node * n = city->getNode(inlet);
        if (n->getAttribute("New")->getDouble() > -1) {
            StartPos.push_back(n);
        }
    }

    //Create Agents

    foreach(DM::Node * p, StartPos) {
        long x = (long) p->getX()/cellSize;
        long y = (long) p->getY()/cellSize;
        AgentExtraxtor * a = new AgentExtraxtor(GenerateSewerNetwork::Pos(x,y));
        a->startNode = p;
        a->Topology = this->Topology;
        a->MarkPath = this->Path;
        a->ConnectivityField = this->ConnectivityField;
        a->ForbiddenAreas = this->ForbiddenAreas;
        a->Goals = this->Goals;
        a->AttractionTopology = 0;
        a->AttractionConnectivity =0;
        a->steps = this->steps;
        agents.push_back(a);

    }
    long successfulAgents = 0;
    double multiplier;
    multiplier = this->ConnectivityField->getCellSize();
    double offset = this->ConnectivityField->getCellSize() /2.;

    //Extract Conduits

    foreach(std::string name, city->getNamesOfComponentsInView(Conduits)) {
        DM::Component * c = city->getComponent(name);
        c->changeAttribute(DM::Attribute("New", 1));
    }

    //CreateEndPointList
    std::vector<DM::Node*> EndPointList;
    foreach(std::string name, this->city->getNamesOfComponentsInView(Junction)) {
        EndPointList.push_back(this->city->getNode(name));
    }


    std::vector<std::vector<Node> > Points_After_Agent_Extraction;
    //Extract Netoworks
    for (int j = 0; j < agents.size(); j++) {
        AgentExtraxtor * a = agents[j];
        if (a->alive) {
            a->run();
        }
        if (a->successful) {
            successfulAgents++;
            std::vector<Node> points_for_total;
            for (int i = 0; i < a->path.size(); i++) {
                this->Path->setValue(a->path[i].x, a->path[i].y, 1);
                points_for_total.push_back(Node(a->path[i].x * multiplier + offset, a->path[i].y * multiplier + offset,a->path[i].h));
            }
            Points_After_Agent_Extraction.push_back(points_for_total);
            //Set Inlet Point to Used

            Node * start = a->startNode;
            start->changeAttribute("Used",1);
            start->changeAttribute("New", 0);
        }

    }

    Logger(Debug) << "Done with the agents Junctions";
    std::vector<std::vector<DM::Node> > PointsToPlace = this->SimplifyNetwork(Points_After_Agent_Extraction, this->ConduitLength/cellSize, offset);

    //Export Inlets
    Logger(Debug) << "Export Junctions";
    std::vector<std::vector<Node *> > Points_For_Conduits;
    foreach (std::vector<Node> pl, PointsToPlace) {
        Node * n = 0;
        n  = TBVectorData::addNodeToSystem2D(city, Inlets, pl[0],offset, false);
        if (n == 0)
            n = city->addNode(pl[0], Junction);
        city->addComponentToView(n, Junction);
        std::vector<DM::Node * > nl;
        nl.push_back(n);
        Points_For_Conduits.push_back(nl);
    }
    std::vector<std::vector<Node *> > Points_For_Conduits_tmp;
    for(int j = 0; j < PointsToPlace.size(); j++){
        std::vector<Node> pl = PointsToPlace[j];
        std::vector<DM::Node * > nl = Points_For_Conduits[j];
        for (int i = 1; i < pl.size(); i++) {
            //Check if Point is already placed
            //If name = 0 Point doesnt exist
            DM::Node * n = TBVectorData::addNodeToSystem2D(city, Junction, pl[i], 0.1);

            if (n->getAttribute("D")->getDouble() > pl[i].getZ()) {
                n->setZ(n->getAttribute("D")->getDouble());
            }

            //int x = pl[i].getX()/cellSize;
            //int y = pl[i].getY()/cellSize;
            //double z = this->Topology->getValue(x,y);
            //n->changeAttribute("Z", z);
            n->changeAttribute("D", pl[i].getZ());
            nl.push_back(n);
        }
        Points_For_Conduits_tmp.push_back(nl);
    }
    Points_For_Conduits = Points_For_Conduits_tmp;

    foreach (std::vector<Node*> pl, Points_For_Conduits) {
        for (int i = 1; i < pl.size(); i++) {
            if (TBVectorData::getEdge(this->city, Conduits, pl[i-1], pl[i]) != 0)
                continue;
            DM::Edge * e = this->city->addEdge(pl[i-1], pl[i], Conduits);
            e->addAttribute("New", 0);
        }
    }


    Logger(DM::Debug) << "Successful " << successfulAgents;
    this->sendDoubleValueToPlot(this->getInternalCounter(), (double) successfulAgents/agents.size());

    for (int j = 0; j < agents.size(); j++) {
        delete agents[j];
    }

    agents.clear();

    foreach (std::string name, this->city->getNamesOfComponentsInView(Junction)) {
        DM::Node * n =this->city->getNode(name);
        int x = n->getX()/cellSize;
        int y = n->getY()/cellSize;
        double z = this->Topology->getValue(x,y);
        n->changeAttribute("Z", z);

    }

    smoothNetwork();

}

std::vector<std::vector<DM::Node> >  ExtractNetwork::SimplifyNetwork(std::vector<std::vector<DM::Node> > &points, int PReduction, double offset) {

    DM::System sys_tmp("");
    DM::View dummy;
    foreach (std::vector<Node> pl, points) {
        bool hitExisting= false;
        foreach (Node node, pl) {
            Node * n = TBVectorData::addNodeToSystem2D(&sys_tmp, dummy, node, offset);
            if (n->getAttribute("Counter")->getDouble() > 0.01) {
                hitExisting = true;
                n->changeAttribute("Counter",100);
                break;
            }
            n->changeAttribute("Counter",1);
        }

    }

    std::vector<std::vector<Node> > ResultVector;
    std::vector<Node> StartandEndPointList;
    //CreateStartAndEndPointList
    foreach (std::vector<Node> pl, points) {
        Node startPoint = pl[0];
        Node endPoint = pl[pl.size()-1];
        StartandEndPointList.push_back(startPoint);
        StartandEndPointList.push_back(endPoint);

    }

    int counter = 0;

    foreach (std::vector<Node> pl, points) {
        std::vector<Node> pointlist_new;

        for (int i  = 0; i < pl.size(); i++) {
            bool placePoint = false;
            Node * n = TBVectorData::getNode2D(&sys_tmp, dummy ,pl[i], offset );

            if (n->getAttribute("Counter")->getDouble() > 99) {
                n->changeAttribute("Counter", n->getAttribute("Counter")->getDouble()+100);
                placePoint = true;
            }

            if (i == 0)
                placePoint = true;
            if (i == pl.size()-1)
                placePoint = true;
            if (counter > PReduction)
                placePoint = true;

            if (placePoint) {
                pointlist_new.push_back(pl[i]);
                counter = 0;
            }
            if (n->getAttribute("Counter")->getDouble() > 201) {
                break;
            }

        }
        ResultVector.push_back(pointlist_new);
    }

    return ResultVector;

}
//TODO: Smooth Networks
void ExtractNetwork::smoothNetwork() {
    std::map<DM::Node *, std::vector<DM::Edge*> > StartNodeSortedEdges;
    std::map<DM::Node *, std::vector<DM::Edge*> > EndNodeSortedEdges;
    std::map<DM::Node *, std::vector<DM::Edge*> > ConnectedEdges;
    std::vector<std::string> InletNames;
    InletNames = city->getNamesOfComponentsInView(this->Inlets);
    std::vector<std::string> ConduitNames;
    ConduitNames = city->getNamesOfComponentsInView(this->Conduits);
    //Create Connection List
    foreach(std::string name , ConduitNames)  {
        DM::Edge * e = city->getEdge(name);
        DM::Node * startnode = city->getNode(e->getStartpointName());
        std::vector<DM::Edge*> v = ConnectedEdges[startnode];
        v.push_back(e);
        ConnectedEdges[startnode] = v;

        DM::Node * Endnode = city->getNode(e->getEndpointName());
        v = ConnectedEdges[Endnode];
        v.push_back(e);
        ConnectedEdges[Endnode] = v;
    }


    foreach(std::string name , ConduitNames)  {
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

    Logger(Debug) << "Start Smoothing Netowrk";
    //find WWTP


   double internalcounter = 0;
    Logger(Debug) << "Endpoint Thing";

    std::vector<DM::Node *> EndPoints;
    foreach (std::string n, this->city->getNamesOfComponentsInView(this->EndPoint))
        EndPoints.push_back(this->city->getNode(n));
    while (EndPoints.size() > 0) {
        std::vector<DM::Node*> new_endPointList;
        foreach (DM::Node * p, EndPoints) {
            if (internalcounter > 100000) {
                Logger(Error) << "Endless loop";
                return;
            }
            internalcounter++;

            double z_lower = p->getAttribute("Z")->getDouble() - p->getAttribute("D")->getDouble();

            std::vector<DM::Edge*> upstreamconnections = EndNodeSortedEdges[p];

            foreach (DM::Edge * e, upstreamconnections)  {

                DM::Node * p_upper = this->city->getNode(e->getStartpointName());
                double z_upper = p_upper->getAttribute("Z")->getDouble() - p_upper->getAttribute("D")->getDouble();
                if (z_lower >= z_upper) {
                    z_upper = z_lower + 0.05;
                }
                internalcounter++;
                if (internalcounter > 100000) {
                    Logger(Error) << "Endless loop";
                    return;
                }
               p_upper->changeAttribute("D", p_upper->getAttribute("Z")->getDouble() - z_upper);
               new_endPointList.push_back(p_upper);
            }
        }
        internalcounter++;
        if (internalcounter > 100000) {
            Logger(Error) << "Endless loop";
            return;
        }
        EndPoints = new_endPointList;
    }
    Logger(Debug) << "Done Endpoint Thing";



}
