/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2011-2012   Christian Urich

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

#include "cutelittlegeometryhelpers.h"
#include "tbvectordata.h"
#include <algorithm>
#include <math.h>

std::vector<DM::Face*> CuteLittleGeometryHelpers::CreateHolesInAWall(DM::System *sys, DM::Face *f, double distance, double width, double height)
{
    std::vector<DM::Node*> nodes = TBVectorData::getNodeListFromFace(sys, f);
    //And again we rotate or wall again into x,y assuming. we assume that z is becoming our new x
    double E[3][3];
    TBVectorData::CorrdinateSystem( DM::Node(0,0,0), DM::Node(1,0,0), DM::Node(0,1,0), E);

    double E_to[3][3];

    TBVectorData::CorrdinateSystem( *(nodes[0]), *(nodes[1]), *(nodes[2]), E_to);


    DM::Node dN1 = *(nodes[1]) - *(nodes[0]);
    DM::Node dN2 = *(nodes[2]) - *(nodes[0]);
    DM::Node orientationOriginal = TBVectorData::NormalVector(dN1, dN2);


    double alphas[3][3];
    TBVectorData::RotationMatrix(E, E_to, alphas);


    DM::System transformedSys;

    std::vector<DM::Node*> ns_t;
    double z_const = 0;
    for (unsigned int i = 0; i < nodes.size(); i++) {
        DM::Node n = *(nodes[i]);
        DM::Node n_t = TBVectorData::RotateVector(alphas, n);
        ns_t.push_back(transformedSys.addNode(n_t));
        z_const = n_t.getZ();
    }

    //Calculate max and min x
    double xmin = ns_t[0]->getX();
    double xmax = ns_t[0]->getX();

    double ymin = ns_t[0]->getY();
    double ymax = ns_t[0]->getY();

    foreach (DM::Node * n, ns_t) {
        DM::Logger(DM::Debug) << n->getX() << "\t" <<  n->getY() << "\t" << n->getZ();
    }

    foreach (DM::Node * n, ns_t) {
        if (xmin > n->getX())
            xmin = n->getX();
        if (xmax < n->getX())
            xmax = n->getX();
        if (ymin > n->getY())
            ymin = n->getY();
        if (ymax < n->getY())
            ymax = n->getY();
    }

    double l = ymax - ymin;

    int numberOfSegments = l / distance;

    std::vector<DM::Node> window_nodes;

    //We will distribute the windows equally as parapet height we asume 0.80 m
    for (unsigned int i = 0; i < numberOfSegments; i++) {
        //Center of the new window
        double delta_y = l / (numberOfSegments+1) * (i+1) + ymin;

        DM::Node n1(xmin + 1.2, delta_y - width/2, z_const);
        DM::Node n2(xmin + 1.2 + height,  delta_y - width/2, z_const);
        DM::Node n3(xmin + 1.2 + height,  delta_y + width/2, z_const);
        DM::Node n4(xmin + 1.2,  delta_y + width/2, z_const);

        window_nodes.push_back(n4);
        window_nodes.push_back(n3);
        window_nodes.push_back(n2);
        window_nodes.push_back(n1);

    }
    //Transform Coordinates back
    double alphas_t[3][3];
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            alphas_t[j][i] =  alphas[i][j];
        }
    }


    std::vector<DM::Face * > windows;

    for (int i = 0; i < window_nodes.size()/4; i++){
        std::vector<DM::Node*> window_nodes_t;
        for (int j = 0; j < 4; j++) {
            DM::Node n = TBVectorData::RotateVector(alphas_t, window_nodes[j+i*4]);
            window_nodes_t.push_back(sys->addNode(n));
        }
        window_nodes_t.push_back(window_nodes_t[0]);


        DM::Node dN1_1 = *(window_nodes_t[1]) - *(window_nodes_t[0]);
        DM::Node dN2_1  = *(window_nodes_t[2]) - *(window_nodes_t[0]);
        DM::Node orientationOriginal_window = TBVectorData::NormalVector(dN1_1, dN2_1);

        DM::Node checkDir = orientationOriginal - orientationOriginal_window;
        if (fabs(checkDir.getX()) > 0.0001 ||
            fabs(checkDir.getY()) > 0.0001 ||
            fabs(checkDir.getZ()) > 0.0001) {
            std::reverse(window_nodes_t.begin(),window_nodes_t.end());
        }

        windows.push_back(sys->addFace(window_nodes_t));

        f->addHole(window_nodes_t);

        DM::Logger(DM::Debug) << "Add Window";
    }
    return windows;
}

void CuteLittleGeometryHelpers::CreateStandardBuilding(DM::System * city, DM::View & buildingView, DM::View & geometryView, DM::Component *BuildingInterface, std::vector<DM::Node * >  & footprint, int stories)
{
    std::vector<double> roofColor;
    roofColor.push_back(0.66);
    roofColor.push_back(0.66);
    roofColor.push_back(0.66);
    std::vector<double> wallColor;
    wallColor.push_back(0.96);
    wallColor.push_back(0.96);
    wallColor.push_back(0.86);
    std::vector<double> windowColor;
    windowColor.push_back(0.5019608);
    windowColor.push_back(1.0);
    windowColor.push_back(0.5019608);

    //Set footprint as floor
    DM::Face * base_plate = city->addFace(footprint, geometryView);
    BuildingInterface->getAttribute("Geometry")->setLink("Geometry", base_plate->getUUID());
    base_plate->getAttribute("Parent")->setLink(buildingView.getName(), BuildingInterface->getUUID());
    base_plate->addAttribute("type", "ceiling_cellar");

    //The Building is extruded stepwise. Housenodes is used as a starting point for the extusion
    std::vector<DM::Node*> houseNodes = footprint;
    //Create Walls
    for (int story = 0; story < stories; story++) {
        std::vector<DM::Face*> extruded_faces = TBVectorData::ExtrudeFace(city, geometryView, houseNodes, 3);
        int lastID = extruded_faces.size();
        for (int i = 0; i < lastID; i++) {
            DM::Face * f = extruded_faces[i];
            if (i != lastID-1) {
                f->addAttribute("type", "wall_outside");
                f->getAttribute("color")->setDoubleVector(wallColor);
                std::vector<DM::Face* > windows = CuteLittleGeometryHelpers::CreateHolesInAWall(city, f, 5, 1.5, 1);
                foreach (DM::Face * w, windows) {
                    w->addAttribute("type", "window");
                    w->getAttribute("color")->setDoubleVector(windowColor);
                    BuildingInterface->getAttribute("Geometry")->setLink("Geometry", w->getUUID());
                    f->getAttribute("Parent")->setLink(buildingView.getName(), BuildingInterface->getUUID());
                    city->addComponentToView(w,geometryView);
                }
            }
            else if (story != stories -1){
                f->addAttribute("type", "ceiling");
                f->getAttribute("color")->setDoubleVector(wallColor);
                houseNodes = TBVectorData::getNodeListFromFace(city, f);
                //Reverse otherwise extruded walls have the wrong orientation
                //std::reverse(houseNodes.begin(), houseNodes.end());
            } else {
                f->addAttribute("type", "ceiling_roof");
                //std::reverse(houseNodes.begin(), houseNodes.end());
                f->getAttribute("color")->setDoubleVector(roofColor);
            }
            BuildingInterface->getAttribute("Geometry")->setLink("Geometry", f->getUUID());
            f->getAttribute("Parent")->setLink(buildingView.getName(), BuildingInterface->getUUID());

        }
    }



}
