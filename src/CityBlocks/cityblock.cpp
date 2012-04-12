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

#include "cityblock.h"
#include <dm.h>
#include <math.h>
#include <tbvectordata.h>

DM_DECLARE_NODE_NAME(CityBlock,BlockCity)

CityBlock::CityBlock()
{
    std::vector<DM::View> views;
    superblock = DM::View("SUPERBLOCK", DM::FACE, DM::READ);
    superblock.addAttribute("CityBlock_Width");
    superblock.addAttribute("CityBlock_Height");
    cityblock = DM::View("CITYBLOCK", DM::FACE, DM::WRITE);
    cityblock.addAttribute("Area");

    streets = DM::View("STREET", DM::EDGE, DM::WRITE);
    intersections = DM::View("INTERSECTION", DM::NODE, DM::WRITE);

    centercityblock = DM::View("CENTERCITYBLOCK", DM::NODE, DM::WRITE);
    centercityblock.addAttribute("ID_CATCHMENT");

    views.push_back(superblock);
    views.push_back(cityblock);
    views.push_back(streets);
    views.push_back(intersections);
    views.push_back(centercityblock);


    this->width = 100;
    this->height = 100;

    this->addParameter("Width", DM::DOUBLE, &this->width);
    this->addParameter("Height", DM::DOUBLE, &this->height);


    this->addData("City", views);
}

void CityBlock::run() {

    DM::System * city = this->getData("City");



    std::vector<std::string> blockids = city->getNamesOfComponentsInView(superblock);

    foreach (std::string blockid, blockids) {
        //calulculate height;

        DM::Face * fblock = city->getFace(blockid);
        double minX = 0;
        double maxX = 0;
        double minY = 0;
        double maxY = 0;
        for (int i = 0; i < fblock->getNodes().size(); i++){

            DM::Node * n1 = city->getNode(fblock->getNodes()[i]);
            DM::Node * n2 = city->getNode(fblock->getNodes()[i]);

            if (i == 0) {
                minX = n1->getX();
                maxX = n1->getX();
                minY = n1->getY();
                maxY = n1->getY();
            }

            if(minX > n1->getX())
                minX = n1->getX();
            if(minX > n2->getX())
                minX = n2->getX();
            if(maxX < n1->getX())
                maxX = n1->getX();
            if(maxX < n2->getX())
                maxX = n2->getX();

            if(minY > n1->getY())
                minY = n1->getY();
            if(minY > n2->getY())
                minY = n2->getY();
            if(maxY < n1->getY())
                maxY = n1->getY();
            if(maxY < n2->getY())
                maxY = n2->getY();

        }
        double blockWidth = maxX - minX;
        double blockHeight = maxY - minY;


        DM::Logger(DM::Debug) << blockHeight;
        DM::Logger(DM::Debug) << blockWidth;


        //Create Parcels

        int elements_x = blockWidth/this->width;
        int elements_y = blockHeight/this->height;
        double realwidth = blockWidth / elements_x;
        double realheight = blockHeight / elements_y;
        fblock->addAttribute("CityBlock_Width",realwidth);
        fblock->addAttribute("CityBlock_Height",realheight);


        for (int x = 0; x < elements_x; x++) {
            for (int y = 0; y < elements_y; y++) {

                DM::Node * n1 = TBVectorData::addNodeToSystem2D(city,
                                                                intersections,
                                                                DM::Node(minX + realwidth*x,minY + realheight*y,0),
                                                                .001);
                DM::Node * n2 = TBVectorData::addNodeToSystem2D(city,
                                                                intersections,
                                                                DM::Node(minX + realwidth*(x+1),minY + realheight*y,0),
                                                                .001);
                DM::Node * n3 = TBVectorData::addNodeToSystem2D(city,
                                                                intersections,
                                                                DM::Node(minX + realwidth*(x+1),minY + realheight*(y+1),0),
                                                                .001);
                DM::Node * n4 = TBVectorData::addNodeToSystem2D(city,
                                                                intersections,
                                                                DM::Node (minX + realwidth*x,minY + realheight*(y+1),0),
                                                                .001);



                DM::Edge * e1 = TBVectorData::getEdge(city, streets, n1, n2, false);
                DM::Edge * e2 = TBVectorData::getEdge(city, streets, n2, n3, false);
                DM::Edge * e3 = TBVectorData::getEdge(city, streets, n3, n4, false);
                DM::Edge * e4 = TBVectorData::getEdge(city, streets, n4, n1, false);

                //Every Edge is also a Street
                if (e1 == 0) {
                    e1 = city->addEdge(n1, n2);
                    city->addComponentToView(e1, streets);
                }
                if (e2 == 0) {
                    e2 = city->addEdge(n2, n3);
                    city->addComponentToView(e2, streets);
                }
                if (e3 == 0) {
                    e3 = city->addEdge(n3, n4);
                    city->addComponentToView(e3, streets);
                }
                if (e4 == 0) {
                    e4 = city->addEdge(n4, n1);
                    city->addComponentToView(e4, streets);
                }

                std::vector<DM::Node*> ve;
                ve.push_back(n1);
                ve.push_back(n2);
                ve.push_back(n3);
                ve.push_back(n4);
                ve.push_back(n1);


                DM::Face * f = city->addFace(ve, cityblock);
                f->addAttribute("Area", realwidth*realheight);
                DM::Node * n =city->addNode(minX + realwidth*(x+0.5),minY + realheight*(y+0.5),0, centercityblock);
                DM::Attribute attr("ID_CATCHMENT");
                attr.setString(f->getName());
                n->addAttribute(attr);
                int i = 0;



            }
        }
    }
    DM::Logger(DM::Debug) << "Number of CityBlocks " << city->getAllComponentsInView(cityblock).size();
    DM::Logger(DM::Debug) << "Number of Streets " << city->getAllComponentsInView(streets).size();

}
