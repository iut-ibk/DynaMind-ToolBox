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

#include "createparcels.h"
#include <DM.h>
#include <math.h>
DM_DECLARE_NODE_NAME(CreateParcels,BlockCity)

CreateParcels::CreateParcels()
{
    std::vector<DM::View> views;
    DM::View block = DM::View("BLOCK", DM::FACE, DM::READ);
    block.addAttribute("Parcel_Width");
    block.addAttribute("Parcel_Height");
    DM::View parcels = DM::View("PARCEL", DM::FACE, DM::WRITE);

    views.push_back(block);
    views.push_back(parcels);


    this->width = 100;
    this->height = 100;

    this->addParameter("Width", DM::DOUBLE, &this->width);
    this->addParameter("Height", DM::DOUBLE, &this->height);


    this->addData("City", views);
}

void CreateParcels::run() {

    DM::System * city = this->getData("City");



    std::vector<std::string> blockids = city->getNamesOfComponentsInView("BLOCK");

    foreach (std::string blockid, blockids) {
        //calulculate height;

        DM::Face * fblock = city->getFace(blockid);
        double minX = 0;
        double maxX = 0;
        double minY = 0;
        double maxY = 0;
        for (int i = 0; i < fblock->getEdges().size(); i++){
            DM::Edge * e = city->getEdge(fblock->getEdges()[i]);
            DM::Node * n1 = city->getNode(e->getStartpointName());
            DM::Node * n2 = city->getNode(e->getEndpointName());

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
        double blockHeight = maxX - minX;
        double blockWidth = maxY - minY;


        DM::Logger(DM::Debug) << blockHeight;
        DM::Logger(DM::Debug) << blockWidth;


        //Create Parcels

        int elements_x = blockWidth/this->height;
        int elements_y = blockHeight/this->width;
        double realwidth = blockWidth / elements_x;
        double realheight = blockHeight / elements_y;
        fblock->addAttribute("Parcel_Width",realwidth);
        fblock->addAttribute("Parcel_Height",realheight);


        for (int x = 0; x < elements_x; x++) {
            for (int y = 0; y < elements_y; y++) {
                DM::Node * n1 = city->addNode(minX + realwidth*x,minY + realheight*y,0);
                DM::Node * n2 = city->addNode(minX + realwidth*(x+1),minY + realheight*y,0);
                DM::Node * n3 = city->addNode(minX + realwidth*(x+1),minY + realheight*(y+1),0);
                DM::Node * n4 = city->addNode(minX + realwidth*x,minY + realheight*(y+1),0);

                DM::Edge * e1 = city->addEdge(n1, n2);
                DM::Edge * e2 = city->addEdge(n2, n3);
                DM::Edge * e3 = city->addEdge(n3, n4);
                DM::Edge * e4 = city->addEdge(n4, n1);

                std::vector<DM::Edge*> ve;
                ve.push_back(e1);
                ve.push_back(e2);
                ve.push_back(e3);
                ve.push_back(e4);


                DM::Face * f = city->addFace(ve, "PARCEL");
            }
        }
    }
    DM::Logger(DM::Debug) << "Number of Parcels " << city->getAllComponentsInView("PARCEL").size();

}
