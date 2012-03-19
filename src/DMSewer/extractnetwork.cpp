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

/*DM_DECLARE_NODE_NAME(ExtractNetwork, Sewer)
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
    //this->Identifier = "";
    this->IdentifierConduit = "Conduit_";
    this->IdentifierEnd = "EndPoint_";
    this->IdentifierInlet = "Inlet_";
    this->IdentifierShaft = "Shaft_";
    //this->addParameter("StartPoints", VIBe2::VECTORDATA_IN, &this->StartPoints);
    //this->addParameter("IdentifierEnd", VIBe2::STRING, &this->IdentifierEnd);
    //this->addParameter("IdentifierConduit", VIBe2::STRING, &this->IdentifierConduit);
    //this->addParameter("IdentifierInlet", VIBe2::STRING, &this->IdentifierInlet);
    //this->addParameter("IdentifierShaft", VIBe2::STRING, &this->IdentifierShaft);
    //this->addParameter("ConnectivityField", VIBe2::RASTERDATA_IN, &this->ConnectivityField);
    //this->addParameter("Goals", VIBe2::RASTERDATA_IN, & this->Goals);
    this->addParameter("Steps", DM::LONG, & this->steps);
    //this->addParameter("Path", VIBe2::RASTERDATA_OUT, &this->Path);
    //this->addParameter("ExportPath", VIBe2::VECTORDATA_OUT, & this->ExportPath,"Shaft_Number_Number|Points  Conduit_Number_Number|Lines ");
    //this->addParameter("ExistingNetwork", VIBe2::VECTORDATA_IN, & this->existingNetwork);
    //this->addParameter("StartPointsOut", VIBe2::VECTORDATA_OUT, & this->StartPointsOut);
    //this->addParameter("Topology", VIBe2::RASTERDATA_IN, &this->Topology);
    //this->addParameter("ForbiddenAreas", VIBe2::RASTERDATA_IN, &this->ForbiddenAreas);
    this->addParameter("ConduitLength", DM::DOUBLE, &this->ConduitLength);

}
void ExtractNetwork::run() {
    long width = this->ConnectivityField->getWidth();
    long height = this->ConnectivityField->getHeight();
    double cellSize = this->ConnectivityField->getCellSize();

    this->Path->setSize(width, height, cellSize);
    this->Path->clear();
    std::vector<AgentExtraxtor * > agents;
    std::vector<std::string> StartPosNames_tmp;
    StartPosNames_tmp = this->StartPoints->getPointsNames();
    std::vector<std::string> StartPosNames;
    foreach(std::string name, StartPosNames_tmp) {
        if (name.find(this->IdentifierInlet,0) == 0) {
            Attribute attr = this->StartPoints->getAttributes(name);
            if ((int)attr.getAttribute("New")  == 1)
                StartPosNames.push_back(name);
        }
    }

    //Create Agents
    foreach(std::string name, StartPosNames) {
        std::vector<Point> points = this->StartPoints->getPoints(name);
        Attribute attr = this->StartPoints->getAttributes(name);

        foreach(Point p, points) {
            long x = (long) p.getX()/cellSize;
            long y = (long) p.getY()/cellSize;
            AgentExtraxtor * a = new AgentExtraxtor(GenerateSewerNetwork::Pos(x,y));
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
    }
    long successfulAgents = 0;
    double multiplier;
    multiplier = this->ConnectivityField->getCellSize();
    double offset = this->ConnectivityField->getCellSize() /2.;
    this->ExportPath->clear();


    *(this->ExportPath) = *(this->existingNetwork);
    *(this->StartPointsOut) = *(this->StartPoints);

    //Extract Conduits

    std::vector<std::string> ExistingConduits = VectorDataHelper::findElementsWithIdentifier(this->IdentifierConduit, this->ExportPath->getEdgeNames());
    foreach(std::string name, ExistingConduits) {
        Attribute attr = this->ExportPath->getAttributes(name);
        attr.setAttribute("Existing", 1);
        this->ExportPath->setAttributes(name, attr);
    }

    //CreateEndPointList
    std::vector<Point> EndPointList;
    std::vector<std::string>EndPosNames = VectorDataHelper::findElementsWithIdentifier(this->IdentifierEnd, this->existingNetwork->getPointsNames());
    foreach(std::string name, EndPosNames) {
        std::vector<Point> ps = this->existingNetwork->getPoints(name);
        foreach(Point p, ps) {
            EndPointList.push_back(p);
        }
    }
    std::vector<Point> EndPoints;
    int conduitcounter = 0;
    int inletcounter = 0;
    int shaftcounter = 0;
    int endpointcounter = 0;


    std::vector<std::string> names = VectorDataHelper::findElementsWithIdentifier(this->IdentifierConduit, this->ExportPath->getEdgeNames());
    foreach (std::string name, names) {
        name.erase(0, this->IdentifierConduit.size());
        int n = atoi(name.c_str());
        if (conduitcounter < n)
            conduitcounter = n;
    }
    conduitcounter++;

    names = VectorDataHelper::findElementsWithIdentifier(this->IdentifierEnd, this->ExportPath->getPointsNames());
    foreach (std::string name, names) {
        name.erase(0, this->IdentifierEnd.size());
        int n = atoi(name.c_str());
        if (endpointcounter < n)
            endpointcounter = n;
    }
    endpointcounter++;

    names = VectorDataHelper::findElementsWithIdentifier(this->IdentifierInlet, this->ExportPath->getPointsNames());
    foreach (std::string name, names) {
        name.erase(0, this->IdentifierInlet.size());
        int n = atoi(name.c_str());
        if (inletcounter < n)
            inletcounter = n;
    }
    inletcounter++;
    names = VectorDataHelper::findElementsWithIdentifier(this->IdentifierShaft, this->ExportPath->getPointsNames());
    foreach (std::string name, names) {
        name.erase(0, this->IdentifierShaft.size());
        int n = atoi(name.c_str());
        if (shaftcounter < n)
            shaftcounter = n;
    }
    shaftcounter++;

    Logger(vibens::Debug) << "Conduits" << conduitcounter;
    Logger(vibens::Debug) << "End" << endpointcounter;
    Logger(vibens::Debug) << "Shaft" << shaftcounter;
    Logger(vibens::Debug) << "Inlet" << inletcounter;

    std::vector<std::vector<Point> > Points_After_Agent_Extraction;
    //Extract Netoworks
    for (int j = 0; j < agents.size(); j++) {
        AgentExtraxtor * a = agents[j];
        if (a->alive) {
            a->run();
        }
        if (a->successful) {
            successfulAgents++;
            std::vector<Point> points_for_total;
            for (int i = 0; i < a->path.size(); i++) {
                this->Path->setValue(a->path[i].x, a->path[i].y, 1);
                points_for_total.push_back(Point(a->path[i].x * multiplier + offset, a->path[i].y * multiplier + offset,a->path[i].h));
            }
            Points_After_Agent_Extraction.push_back(points_for_total);
            //Set Inlet Point to Used
            Attribute attr = this->StartPointsOut->getAttributes(StartPosNames[j]);
            attr.setAttribute("Used", 1);
            attr.setAttribute("New", 0);
            this->StartPointsOut->setAttributes(StartPosNames[j], attr);

        }

    }


    std::vector<std::vector<Point> > PointsToPlace = this->SimplifyNetwork(Points_After_Agent_Extraction, this->ConduitLength/cellSize, offset);

    //Export Inlets
    Logger(Debug) << "Export Inlets";
    foreach (std::vector<Point> pl, PointsToPlace) {        
        Point point(pl[0]);
        std::stringstream ss1;
        ss1 << this->IdentifierInlet << inletcounter;
        std::vector<Point> inlets;
        inlets.push_back(point);
        Attribute attr = VectorDataHelper::findAttributeFromPoints(*(this->ExportPath), point, this->IdentifierInlet, offset);


        foreach(std::string name, StartPosNames) {
            std::vector<Point> points = this->StartPoints->getPoints(name);
            //Find Existing Inlets and Append Data
            foreach(Point p, points) {
                if (p.compare2d(point, offset)) {
                    Attribute att;
                    att = this->StartPoints->getAttributes(name);
                    std::vector<std::string> attrNames = att.getAttributeNames();
                    foreach (std::string name, attrNames) {
                        attr.setAttribute(name, att.getAttribute(name));
                    }
                }
            }
        }
        this->ExportPath->setPoints(ss1.str(), inlets);
        this->ExportPath->setAttributes(ss1.str(), attr);
        inletcounter++;
    }
    //Export Shafts
    Logger(Debug) << "Export Shafts";
    foreach (std::vector<Point> pl, PointsToPlace) {

        for (int i = 0; i < pl.size(); i++) {

            //Check if Point is already placed
            //If name = 0 Point doesnt exist
            std::string name = VectorDataHelper::findPointID(*(this->ExportPath), pl[i], this->IdentifierShaft);
            if (name.size() == 0) {
                std::stringstream ss;
                ss << this->IdentifierShaft<< shaftcounter;
                std::vector<Point> pl_new;
                pl_new.push_back(pl[i]);
                //Get Height

                name = ss.str();
                this->ExportPath->setPoints(name,pl_new);

            } else {
                if (  this->ExportPath->getAttributes(name).getAttribute("D") > pl[i].getZ()) {
                    Point p =  pl[i];
                    p.z = this->ExportPath->getAttributes(name).getAttribute("D");
                    pl[i] = p;


                }
            }
            int x = pl[i].getX()/cellSize;
            int y = pl[i].getY()/cellSize;
            double z = this->Topology->getValue(x,y);
            Attribute attr = this->ExportPath->getAttributes(name);
            attr.setAttribute("Z", z);
            attr.setAttribute("D", pl[i].getZ());
            this->ExportPath->setAttributes(name, attr);

            shaftcounter++;
        }
    }

    //Export EndPoints
    Logger(Debug) << "Export Endpoints";
    foreach (std::vector<Point> pl, PointsToPlace) {
        Point point = *pl.end();
        foreach(Point p, EndPointList) {
            if (p.compare2d(point) == true) {

                //Check if Point Exists
                bool exists = false;
                foreach (Point ep, EndPoints) {
                    if (ep.compare2d(point))
                        exists = true;
                }
                if (!exists) {
                    std::stringstream ss1;
                    ss1 << "EndPoint_"   << endpointcounter;
                    std::vector<Point> endpoints;
                    endpoints.push_back(point);

                    Attribute attr;
                    foreach(std::string name, EndPosNames) {
                        std::vector<Point> points = this->existingNetwork->getPoints(name);
                        foreach(Point p, points) {
                            if (p.compare2d(point, offset))
                                attr = this->existingNetwork->getAttributes(name);
                        }

                    }
                    this->ExportPath->setPoints(ss1.str(), endpoints);
                    this->ExportPath->setAttributes(ss1.str(), attr);
                    endpointcounter++;
                }
            }
        }
    }
    //Export Conduits
    Logger(Debug) << "Export Conduits";
    foreach (std::vector<Point> pl, PointsToPlace) {
        for (int i = 1; i < pl.size(); i++) {
            //Check if Point exists
            if (VectorDataHelper::findEdgeID(*(this->ExportPath), pl[i-1], pl[i], this->IdentifierConduit, offset).size() > 0)
                continue;
            std::stringstream ss;
            ss << this->IdentifierConduit << conduitcounter;
            std::vector<Point> points;
            points.push_back(pl[i-1]);
            points.push_back(pl[i]);
            std::vector<Edge> edges;
            edges.push_back(Edge(0,1));
            this->ExportPath->setPoints(ss.str(),points);
            this->ExportPath->setEdges(ss.str(),edges );
            conduitcounter++;
        }
    }


    Logger(vibens::Debug) << "Successful " << successfulAgents;
    this->sendDoubleValueToPlot(this->getInternalCounter(), (double) successfulAgents/agents.size());

    for (int j = 0; j < agents.size(); j++) {
        delete agents[j];
    }

    agents.clear();


    smoothNetwork();

}

vector ExtractNetwork::SimplifyNetwork(vector &points, int PReduction, double offset) {
    std::vector<std::vector<Point> > ResultVector;
    std::vector<Point> StartandEndPointList;
    //CreateStartAndEndPointList
    foreach (std::vector<Point> pl, points) {
        Point startPoint = pl[0];
        Point endPoint = pl[pl.size()-1];
        StartandEndPointList.push_back(startPoint);
        StartandEndPointList.push_back(endPoint);

    }

    int counter = 0;
    foreach (std::vector<Point> pl, points) {
        std::vector<Point> pointlist_new;
        for (int i  = 0; i < pl.size(); i++) {
            counter++;
            bool placePoint = false;
            if (VectorDataHelper::checkIfPointExists(StartandEndPointList, pl[i], offset))
                placePoint = true;
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

        }
        ResultVector.push_back(pointlist_new);
    }

    return ResultVector;

}

void ExtractNetwork::smoothNetwork() {
    Logger(Debug) << "Start Smoothing Netowrk";
    //find WWTP
    std::vector<std::string> JunctionNames = VectorDataHelper::findElementsWithIdentifier(this->IdentifierShaft, this->ExportPath->getPointsNames());

    std::vector<Point> EndPoints;
    foreach (std::string name, JunctionNames) {
        Attribute attr = this->ExportPath->getAttributes(name);
        if (attr.getAttribute("WWTP") > 0 ) {
            EndPoints.push_back(this->ExportPath->getPoints(name)[0]);
        }
    }
    double internalcounter = 0;
    Logger(Debug) << "Endpoint Thing";
    while (EndPoints.size() > 0) {

        std::vector<Point> new_endPointList;
        foreach (Point p, EndPoints) {
            if (internalcounter > 100000) {
                Logger(Error) << "Endless loop";
                return;
            }
            internalcounter++;
            std::string point_id = VectorDataHelper::findPointID(*(this->ExportPath), p, this->IdentifierShaft, 10);
            Attribute attr = this->ExportPath->getAttributes(point_id);
            double z_lower = attr.getAttribute("Z") - attr.getAttribute("D");

            std::vector<std::string> identifer_cons = VectorDataHelper::findConnectedEges(*(this->ExportPath), p, 10, UPPER, this->IdentifierConduit);
            foreach (std::string name, identifer_cons)  {

                Point p_upper = this->ExportPath->getPoints(name)[0];
                std::string point_id = VectorDataHelper::findPointID(*(this->ExportPath), p_upper, this->IdentifierShaft, 10);
                Attribute attr = this->ExportPath->getAttributes(point_id);
                double z_upper = attr.getAttribute("Z") - attr.getAttribute("D");

                if (z_lower >= z_upper) {
                    z_upper = z_lower + 0.05;
                }
                internalcounter++;
                if (internalcounter > 100000) {
                    Logger(Error) << "Endless loop";
                    return;
                }
                attr.setAttribute("D", attr.getAttribute("Z") - z_upper);
                this->ExportPath->setAttributes(point_id, attr);
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



}*/
