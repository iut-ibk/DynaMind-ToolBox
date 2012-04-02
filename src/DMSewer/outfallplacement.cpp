/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2011-2012  Christian Urich

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
#include "outfallplacement.h"
#include <dmhelper.h>
DM_DECLARE_NODE_NAME(OutfallPlacement, Sewer)
OutfallPlacement::OutfallPlacement()
{
    Junction = DM::View("JUNCTION", DM::NODE, DM::READ);
    Conduit = DM::View("CONDUIT", DM::EDGE, DM::MODIFY);
    Conduit.getAttribute("Strahler");
    Storage = DM::View("STORAGE", DM::NODE, DM::WRITE);
    Storage.addAttribute("Storage");
    Inlet = DM::View("INLET", DM::NODE, DM::READ);
    Inlet.addAttribute("Outfall");
    WWTP = DM::View("WWTP", DM::NODE, DM::READ);
    Outfall = DM::View("OUTFALL", DM::NODE, DM::WRITE);
    ConduitOutfall = DM::View("WEIR", DM::EDGE, DM::WRITE);

    MaxStrahler[1] = 100;
    MaxStrahler[2] = 0;
    MaxStrahler[3] = 0;

    StrahlerDifferenz[1] = 0;
    StrahlerDifferenz[2] = 0;
    StrahlerDifferenz[3] = 100;


    MaxStrahlerStorage[1] = 100;
    MaxStrahlerStorage[2] = 0;
    MaxStrahlerStorage[3] = 0;

    StrahlerDifferenzStorage[1] = 0;
    StrahlerDifferenzStorage[2] = 0;
    StrahlerDifferenzStorage[3] = 100;

    StringMaxStrahler = DMHelper::convertIntMapToStringMap(MaxStrahler);
    StringStrahlerDifferenz = DMHelper::convertIntMapToStringMap(StrahlerDifferenz);


    this->addParameter("MaxStrahler", DM::STRING_MAP, &StringMaxStrahler);
    this->addParameter("StrahlerDifferenz", DM::STRING_MAP, &StringStrahlerDifferenz);

    this->addParameter("MaxStrahlerStorage", DM::STRING_MAP, &StringMaxStrahlerStorage);
    this->addParameter("StrahlerDifferenzStorage", DM::STRING_MAP, &StringStrahlerDifferenzStorage);

    std::vector<DM::View> sewer;

    sewer.push_back(Junction);
    sewer.push_back(Conduit);
    sewer.push_back(Inlet);
    sewer.push_back(WWTP);
    sewer.push_back(Outfall);
    sewer.push_back(ConduitOutfall);
    sewer.push_back(Storage);



    this->addData("city", sewer);

    ForbiddenAreas = DM::View("ForbiddenAreas", DM::RASTERDATA, DM::READ);

    std::vector<DM::View> forbidden;
    forbidden.push_back(ForbiddenAreas);
    this->addData("ForbiddenAreas", forbidden);



}

void OutfallPlacement::run() {
    MaxStrahler = DMHelper::convertStringMapToIntMap(StringMaxStrahler);
    StrahlerDifferenz =  DMHelper::convertStringMapToIntMap(StringStrahlerDifferenz);

    MaxStrahlerStorage = DMHelper::convertStringMapToIntMap(StringMaxStrahlerStorage);
    StrahlerDifferenzStorage =  DMHelper::convertStringMapToIntMap(StringStrahlerDifferenzStorage);

    this->city = this->getData("city");

    std::vector<std::string> ConduitNames;
    ConduitNames = city->getNamesOfComponentsInView(Conduit);

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


    int Strahler_Max = 0;

    //Get Highest Strahler Number
    DM::ComponentMap cmp = city->getAllComponentsInView(Conduit);
    for (DM::ComponentMap::const_iterator it = cmp.begin(); it != cmp.end(); ++it){
        DM::Component * c = it->second;
        if (Strahler_Max < c->getAttribute("Strahler")->getDouble())
            Strahler_Max = c->getAttribute("Strahler")->getDouble();
    }


    //names = VectorDataHelper::findElementsWithIdentifier(this->Identifier_Inlet, this->Network->getPointsNames());

    std::vector<DM::Node * > New_Outfalls;
    foreach (std::string s, this->city->getNamesOfComponentsInView(WWTP)) {
        New_Outfalls.push_back(this->city->getNode(s));
    }

    foreach ( std::string name, this->city->getNamesOfComponentsInView(Inlet)) {
        DM::Node * p = this->city->getNode(name);
        //Get Downstream Node
        std::vector<DM::Edge*> connectedConduits = this->StartNodeSortedEdges[p];
        int prevStrahler = 1;
        //std::vector<std::string> VisitedConduits;
        while (connectedConduits.size() == 1) {
            DM::Edge * e = connectedConduits[0];
            int currentStrahler = e->getAttribute("Strahler")->getDouble();
            //Check for Placement after MaxStrahlerNumber
            int chooser = rand() % 100+1;
            int deltaStrahler = -1;
            //Search for biggest difference
            for (std::map<int, int>::const_iterator it = this->MaxStrahler.begin(); it!= this->MaxStrahler.end(); ++it) {
                if (it->second < chooser)
                    continue;
                if (deltaStrahler == -1 || deltaStrahler < it->first) {
                    deltaStrahler = it->first;
                }
            }


            if (currentStrahler > prevStrahler
                    && Strahler_Max - deltaStrahler <= currentStrahler && deltaStrahler > -1) {
                if (find(New_Outfalls.begin(), New_Outfalls.end(), p) == New_Outfalls.end()) {
                    DM::Logger(DM::Debug) << "Place Outfall";
                    //Check if Outfall Exists
                    if (p->getAttribute("Outfall")->getDouble() < 1) {
                        p->changeAttribute("Outfall", 1);
                        New_Outfalls.push_back(p);
                        //city->addComponentToView(p, Outfall);
                    }
                }
            }

            chooser = rand() % 100+1;
            deltaStrahler = -1;
            //Search for smallest difference
            for (std::map<int, int>::const_iterator it = this->StrahlerDifferenz.begin(); it!= this->StrahlerDifferenz.end(); ++it) {
                if (it->second < chooser)
                    continue;
                if (deltaStrahler == -1 || deltaStrahler > it->first) {
                    deltaStrahler = it->first;
                }
            }

            if (currentStrahler > prevStrahler
                    && deltaStrahler <= currentStrahler-prevStrahler && deltaStrahler > -1 ) {
                if (find(New_Outfalls.begin(), New_Outfalls.end(), p) == New_Outfalls.end()) {
                    DM::Logger(DM::Debug) << "Place Outfall";
                    //Check if Outfall Exists
                    if (p->getAttribute("Outfall")->getDouble() < 1) {
                        p->changeAttribute("Outfall", 1);
                        New_Outfalls.push_back(p);
                    }
                }
            }

            /** Storage **/
            //Check for Placement after MaxStrahlerNumber
            //chooser = rand() % 100+1;
            deltaStrahler = -1;
            //Search for biggest difference
            for (std::map<int, int>::const_iterator it = this->MaxStrahlerStorage.begin(); it!= this->MaxStrahlerStorage.end(); ++it) {
                if (it->second < chooser)
                    continue;
                if (deltaStrahler == -1 || deltaStrahler < it->first) {
                    deltaStrahler = it->first;
                }
            }


            if (currentStrahler > prevStrahler
                    && Strahler_Max - deltaStrahler <= currentStrahler && deltaStrahler > -1) {
                    DM::Logger(DM::Debug) << "Place Outfall";
                    //Check if Outfall Exists
                    if (p->getAttribute("Outfall")->getDouble() < 1) {

                        New_Outfalls.push_back(p);
                    }
                     p->changeAttribute("Outfall", 1);
                     city->addComponentToView(p, Storage);
                     p->addAttribute("Storage", 1);
            }

            chooser = rand() % 100+1;
            deltaStrahler = -1;
            //Search for smallest difference
            for (std::map<int, int>::const_iterator it = this->StrahlerDifferenzStorage.begin(); it!= this->StrahlerDifferenzStorage.end(); ++it) {
                if (it->second < chooser)
                    continue;
                if (deltaStrahler == -1 || deltaStrahler > it->first) {
                    deltaStrahler = it->first;
                }
            }

            if (currentStrahler > prevStrahler
                    && deltaStrahler <= (currentStrahler-prevStrahler) && deltaStrahler > -1) {
                    DM::Logger(DM::Debug) << "Place Outfall";
                    //Check if Outfall Exists
                    if (p->getAttribute("Outfall")->getDouble() < 1) {

                        New_Outfalls.push_back(p);
                    }
                     p->changeAttribute("Outfall", 1);
                     p->addAttribute("Storage", 1);
                     city->addComponentToView(p, Storage);
            }







            p = this->city->getNode(e->getEndpointName());
            prevStrahler = currentStrahler;
            connectedConduits = this->StartNodeSortedEdges[p];
        }
    }

    //Create Outfalls and Weirs
    foreach(DM::Node * p, New_Outfalls) {
        DM::Node OffestPoint(30,30,0);
        OffestPoint = OffestPoint + *(p);
        DM::Node * of = this->city->addNode(OffestPoint, Outfall);
        of->addAttribute("Z", p->getAttribute("Z")->getDouble()-4);
        this->city->addEdge(p, of, ConduitOutfall);
    }

    //Conduit to WWTP
    std::vector<std::string> wwtps = this->city->getNamesOfComponentsInView(WWTP);
    foreach(std::string wwtp, wwtps) {
        DM::Node * p = this->city->getNode(wwtp);
        DM::Node OffestPoint(-30,-30,0);
        OffestPoint = OffestPoint + *(p);
        DM::Node * of = this->city->addNode(OffestPoint, Outfall);
        of->addAttribute("WWTP", 1);
        of->addAttribute("Z", p->getAttribute("Z")->getDouble()-4);
        DM::Edge * e = this->city->addEdge(p, of, Conduit);
        e->addAttribute("WWTP",100);

    }
}
