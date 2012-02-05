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
    catchment =  DM::View("CATCHMENT", DM::FACE, DM::WRITE);
    catchment.addAttribute("ID_INLET");

    std::vector<DM::View> views;

    views.push_back(intersections);
    views.push_back(streets);
    views.push_back(conduits);
    views.push_back(inlets);
    views.push_back(endnode);
    views.push_back(catchment);

    this->addData("City", views);

}

void SimpleSewerLayout::run() {

    DM::System * city = this->getData("City");

    DM::Node * EndNode = TBVectorData::getNode2D(city,
                                               intersections,
                                               DM::Node(0,500,0),
                                               0);


    std::vector<DM::Edge*> edg;
    DM::Face * f= city->addFace(edg, catchment);
    f->addAttribute("Population", 1000);
    f->addAttribute("Area", 1000);
    f->addAttribute("Impervious", 1);
    f->addAttribute("WasteWater", 100);


    city->addComponentToView(EndNode, endnode);
    city->addComponentToView(EndNode, shaft);


    DM::Node * n1 = city->addNode(100, 500,0, shaft);

    DM::Node * n2 = city->addNode(100, 1000,0, inlets);
    DM::Attribute  attr1 = DM::Attribute("ID_CATCHMENT");
    attr1.setString(f->getName());
    n2->addAttribute(attr1);
    DM::Node * n3 = city->addNode(100, 0,0, inlets);
    DM::Attribute attr =  DM::Attribute("ID_CATCHMENT");
    attr.setString(f->getName());
    n3->addAttribute(attr);
    city->addEdge(n1, EndNode, conduits);
    city->addEdge(n2, n1, conduits);
    city->addEdge(n3, n1, conduits);









}
