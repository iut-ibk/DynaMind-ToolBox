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
/**
  * Input Shape
  *
  * 3----2
  * |    |
  * 0----1
  *
  */
void Parcels::run() {
    DM::System * city = this->getData("City");
    std::vector<std::string> cityblocks = city->getNamesOfComponentsInView(cityblock);
    foreach (std::string idblock, cityblocks) {
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
        double height = delta.getY();



        //Add Street
        DM::Node * streetnode1 = city->addNode(origin.getX(), origin.getY()+height/2, 0, intersections);
        DM::Node * streetnode2 = city->addNode(origin.getX()+length, origin.getY()+height/2, 0, intersections);

        DM::Edge * street1 = TBVectorData::getEdge(city, streets, nodelist[0], nodelist[3], false);
        if (street1 != 0) {
            TBVectorData::splitEdge(city,  street1, streetnode1, streets);
        }
        DM::Edge * street2 = TBVectorData::getEdge(city, streets, nodelist[1], nodelist[2], false);
        if (street2 != 0) {
            TBVectorData::splitEdge(city,  street2, streetnode2, streets);
        }

        DM::Edge * street = city->addEdge(streetnode1, streetnode2, streets);
        street->addAttribute("Order",1);







        //Create Parcels

        int elements_x = length/20;
        int elements_y = height/10;
        double realwidth = length / elements_x;
        double realheight = height / elements_y;
        bool generateStreet = true;

        /**Generation Process -> left to right
      *     |    |
      * ----|----|
      *     |    |
      *     1    2
    */
        for (int x = 0; x < elements_x; x++) {

            for (int y = 0; y < elements_y; y++) {

                /*DM::Node * n1 = TBVectorData::addNodeToSystem2D(city,
                                                                intersections,
                                                                origin+DM::Node(realwidth*x,realheight*y,0),
                                                                true,
                                                                .001);
                DM::Node * n2 = TBVectorData::addNodeToSystem2D(city,
                                                                intersections,
                                                                origin+DM::Node(realwidth*(x+1), realheight*y,0),
                                                                true,
                                                                .001);
                DM::Node * n3 = TBVectorData::addNodeToSystem2D(city,
                                                                intersections,
                                                                origin+DM::Node( realwidth*(x+1), realheight*(y+1),0),
                                                                true,
                                                                .001);
                DM::Node * n4 = TBVectorData::addNodeToSystem2D(city,
                                                                intersections,
                                                                origin+DM::Node (realwidth*x,realheight*(y+1),0),
                                                                true,
                                                                .001);*/
                DM::Node * n1 = city->addNode( origin+DM::Node(realwidth*x,realheight*y,0));
                DM::Node * n2 = city->addNode( origin+DM::Node(realwidth*(x+1), realheight*y,0));
                DM::Node * n3 = city->addNode( origin+DM::Node( realwidth*(x+1), realheight*(y+1),0));
                DM::Node * n4 = city->addNode( origin+DM::Node (realwidth*x,realheight*(y+1),0));




                DM::Edge * e1 = city->addEdge(n1, n2);

                DM::Edge * e2 = city->addEdge(n2, n3);

                DM::Edge * e3 = city->addEdge(n3, n4);

                DM::Edge * e4 = city->addEdge(n4, n1);


                //Every Edge is also a Street
                /*if (TBVectorData::getEdge(city, streets, e1, false) == 0) {
                    city->addComponentToView(e1, streets);
                    e1->addAttribute("Order", 2);


                }*/
                if (generateStreet) {
                    city->addComponentToView(e2, streets);
                    e2->addAttribute("Order", 2);
                    if (y == 0)
                        TBVectorData::splitEdge(city, street, n2, streets);

                }
                /*if (TBVectorData::getEdge(city, streets, e3, false) == 0) {
                    city->addComponentToView(e3, streets);
                    e3->addAttribute("Order", 2);

                }
                if (TBVectorData::getEdge(city, streets, e4, false) == 0) {
                    city->addComponentToView(e4, streets);
                    e4->addAttribute("Order", 2);

                }*/

                std::vector<DM::Node*> ve;
                ve.push_back(n1);
                ve.push_back(n2);
                ve.push_back(n3);
                ve.push_back(n4);


                DM::Face * f = city->addFace(ve, parcels);
            }
            if (generateStreet) {
                generateStreet = false;
            } else {
                generateStreet = true;
            }
        }
    }


    DM::Logger(DM::Debug) << "Number of Parcels " << city->getAllComponentsInView(parcels).size();
    DM::Logger(DM::Debug) << "Number of Streets " << city->getAllComponentsInView(streets).size();


}
