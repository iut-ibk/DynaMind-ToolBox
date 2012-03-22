/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2012  Christian Urich
 
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

#include "simplesewerlayout.h"
#include "tbvectordata.h"

DM_DECLARE_NODE_NAME(SimpleSewerLayout,BlockCity)

SimpleSewerLayout::SimpleSewerLayout()
{
    intersections = DM::View("INTERSECTION", DM::EDGE, DM::READ);
    streets = DM::View("STREET", DM::EDGE, DM::READ);
    conduits = DM::View("CONDUIT", DM::EDGE, DM::WRITE);
    inlets =  DM::View("INLET", DM::EDGE, DM::WRITE);
    inlets.addAttribute("ID_CATCHMENT");
    shaft = DM::View("SHAFT", DM::NODE, DM::READ);
    endnode =  DM::View("ENDNODE", DM::EDGE, DM::WRITE);
    cityblock =  DM::View("CITYBLOCK", DM::FACE, DM::READ);
    catchment =  DM::View("CATCHMENT", DM::FACE, DM::WRITE);
    wwtp =  DM::View("WWTP", DM::NODE, DM::WRITE);
    catchment.addAttribute("ID_INLET");

    std::vector<DM::View> views;

    views.push_back(intersections);
    views.push_back(streets);
    views.push_back(cityblock);
    views.push_back(conduits);
    views.push_back(inlets);
    views.push_back(endnode);
    views.push_back(catchment);
    views.push_back(wwtp);

    this->addData("City", views);

}

void SimpleSewerLayout::run() {

    DM::System * city = this->getData("City");




    std::vector<std::string> cityblocks = city->getNamesOfComponentsInView(cityblock);


    //Define Faces
    foreach (std::string block, cityblocks) {
        DM::Face * face = city->getFace(block);
        city->addComponentToView(face, catchment);
        face->addAttribute("Population", 1000);
        face->addAttribute("Area", 10000);
        face->addAttribute("Impervious", 1);
        face->addAttribute("WasteWater", 100);
        face->addAttribute("Gradient", 1/5000.);
        std::vector<std::string> nodes = face->getNodes();

        DM::Node * n1 = city->getNode(nodes[0]);

        city->addComponentToView(n1, inlets);
        DM::Attribute  attr1 = DM::Attribute("ID_CATCHMENT");
        attr1.setString(face->getName());
        n1->addAttribute(attr1);



    }


    DM::Node * EndNode = city->addNode(-100,500,0, endnode);
    city->addComponentToView(EndNode, shaft);
    city->addComponentToView(EndNode, wwtp);

    DM::Node * StartNode=  TBVectorData::getNode2D(city, intersections, DM::Node(0, 500, 0), 0);
    city->addEdge(StartNode , EndNode, conduits);
    city->addComponentToView(StartNode, shaft);
    this->addConduit(city, StartNode, 2);



}

/**
  * Directions
  *             1
  *       4           2
  *             3
  */
void SimpleSewerLayout::addConduit(DM::System* city, DM::Node * n, int direction) {
    DM::Node * prev = n;

    if (direction == 2) {
        this->addConduit(city, n, 1);
        this->addConduit(city, n, 3);
    }
    n = this->connectNextInlet(city, n, direction);
    if (n == 0)
        return;
    if (direction == 2 || direction == 4) {
        if( n->getX() == prev->getX()) {
            n = 0;
        }
    }

    if (direction ==  1|| direction == 3) {
        if( n->getY() == prev->getY()) {
            n = 0;
        }
    }

    if (n != 0) {
        city->addEdge( n,prev, conduits);
        this->addConduit(city, n, direction);
        city->addComponentToView(n, shaft);
    }

}

DM::Node * SimpleSewerLayout::connectNextInlet(DM::System * city, DM::Node * n1,  int direction) {

    std::vector<DM::Edge*> connectedEdges = TBVectorData::getConnectedEdges(city, streets, *(n1), 0);

    std::vector<DM::Node*> searchList;
    std::vector<DM::Edge*> searchEdges;
    foreach (DM::Edge * e, connectedEdges) {
        DM::Node * n_1 = city->getNode(e->getStartpointName());
        DM::Node * n_2 = city->getNode(e->getEndpointName());

        if (!n_1->compare2d(n1)) {
            searchList.push_back(n_1);
            searchEdges.push_back(e);
        }
        if (!n_2->compare2d(n1)) {
            searchList.push_back(n_2);
            searchEdges.push_back(e);
        }
    }
    double minx;
    int IDminx;

    double maxx;
    int IDmaxx;

    double miny;
    int IDminy;

    double maxy;
    int IDmaxy;

    for (int i = 0; i < searchList.size(); i++){
        if (i == 0) {
            IDminx = 0;
            minx = searchList[i]->getX();
            IDmaxx = 0;
            maxx = searchList[i]->getX();
            IDminy = 0;
            miny= searchList[i]->getY();
            IDmaxy = 0;
            maxy = searchList[i]->getY();
            continue;
        }

        if (searchList[i]->getX() < minx) {
            IDminx = i;
            minx = searchList[i]->getX();
        }
        if (searchList[i]->getX() > maxx) {
            IDmaxx = i;
            maxx = searchList[i]->getX();
        }
        if (searchList[i]->getY() < miny) {
            IDminy = i;
            miny = searchList[i]->getY();
        }
        if (searchList[i]->getY() > maxy) {
            IDmaxy= i;
            maxy = searchList[i]->getY();
        }

    }


    if (searchList.size() != 0) {
        if (direction == 1 && maxy > n1->getY())
            return searchList[IDmaxy];
        if (direction == 2 && maxx > n1->getX())
            return searchList[IDmaxx];
        if (direction == 3 && miny < n1->getY())
            return searchList[IDminy];
        if (direction == 4 && minx < n1->getX())
            return searchList[IDminx];

    }
    return 0;


}
