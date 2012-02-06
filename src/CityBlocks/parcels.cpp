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

#include "parcels.h"
#include <tbvectordata.h>

DM_DECLARE_NODE_NAME(Parcels,BlockCity)

Parcels::Parcels()
{
    cityblock = DM::View("CITYBLOCK", DM::FACE, DM::READ);

    parcels = DM::View("PARCEL", DM::FACE, DM::READ);

    streets = DM::View("STREET", DM::EDGE, DM::MODIFY);
    streets.addAttribute("Order");
    intersections = DM::View("INTERSECTION", DM::NODE, DM::MODIFY);
    std::vector<DM::View> views;


    views.push_back(cityblock);
    views.push_back(parcels);
    views.push_back(streets);

    this->addData("City", views);

}

void Parcels::run() {
    DM::System * city = this->getData("City");

    foreach (std::string idblock, city->getNamesOfComponentsInView(cityblock)) {
        DM::Face * block = city->getFace(idblock);
        //Get minx and miny as origin
        double minx;
        double miny;
        double maxx;
        double maxy;
        std::vector<DM::Node *> nodelist =  TBVectorData::getNodeListFromFace(city, block);
        for (int i = 0; i < nodelist.size(); i++) {
            if (i == 0) {
                minx = nodelist[i]->getX();
                miny = nodelist[i]->getY();
                maxx = nodelist[i]->getX();
                maxy = nodelist[i]->getY();
                continue;
            }
            if (minx > nodelist[i]->getX())
                minx = nodelist[i]->getX();
            if (miny > nodelist[i]->getY())
                miny = nodelist[i]->getY();
            if (maxx < nodelist[i]->getX())
                maxx = nodelist[i]->getX();
            if (maxy < nodelist[i]->getY())
                maxy = nodelist[i]->getY();
        }
        DM::Node origin = DM::Node(minx, miny, 0);
        DM::Node delta = DM::Node(maxx, maxy,0) -origin;
        double length = delta.getX();
        double height =  delta.getY();

        //Add Street
        DM::Node * streetnode1 = city->addNode(origin.getX(), origin.getY()+height/2, 0, intersections);
        DM::Node * streetnode2 = city->addNode(origin.getX()+length, origin.getY()+height/2, 0, intersections);



    }





}
