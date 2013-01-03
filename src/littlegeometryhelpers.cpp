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

#include "littlegeometryhelpers.h"
#include "cgalgeometry.h"
#include "tbvectordata.h"
#include <algorithm>
#include <math.h>
#include <QPointF>
#include <QPolygonF>
#include <QTransform>

std::vector<DM::Face*> LittleGeometryHelpers::CreateHolesInAWall(DM::System *sys, DM::Face *f, double distance, double width, double height, double parapet)
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
    for (int i = 0; i < numberOfSegments; i++) {
        //Center of the new window
        double delta_y = l / (numberOfSegments+1) * (i+1) + ymin;

        DM::Node n1(xmin + parapet, delta_y - width/2, z_const);
        DM::Node n2(xmin + parapet + height,  delta_y - width/2, z_const);
        DM::Node n3(xmin + parapet + height,  delta_y + width/2, z_const);
        DM::Node n4(xmin + parapet,  delta_y + width/2, z_const);

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

    for ( unsigned int i = 0; i < window_nodes.size()/4; i++){
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

        //DM::Logger(DM::Debug) << "Add Window";
    }
    return windows;
}

void LittleGeometryHelpers::CreateStandardBuilding(DM::System * city, DM::View & buildingView, DM::View & geometryView, DM::Component *BuildingInterface, std::vector<DM::Node * >  & footprint, int stories, bool createWindows)
{
    std::vector<double> roofColor;
    roofColor.push_back(0.66);
    roofColor.push_back(0.66);
    roofColor.push_back(0.66);
    std::vector<double> wallColor;
    wallColor.push_back(196./255.);
    wallColor.push_back(196./255.);
    wallColor.push_back(196./255.);
    std::vector<double> windowColor;
    windowColor.push_back(204./255.);
    windowColor.push_back(229./255.);
    windowColor.push_back(1);

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
                if (createWindows){
                    std::vector<DM::Face* > windows = LittleGeometryHelpers::CreateHolesInAWall(city, f, 5, 1.5, 1);
                    foreach (DM::Face * w, windows) {
                        w->addAttribute("type", "window");
                        w->getAttribute("color")->setDoubleVector(windowColor);
                        BuildingInterface->getAttribute("Geometry")->setLink("Geometry", w->getUUID());
                        w->getAttribute("Parent")->setLink(buildingView.getName(), BuildingInterface->getUUID());
                        city->addComponentToView(w,geometryView);
                    }
                }

            }
            else if (story != stories -1){
                f->addAttribute("type", "ceiling");
                f->getAttribute("color")->setDoubleVector(wallColor);
                houseNodes = TBVectorData::getNodeListFromFace(city, f);
            } else {
                /*std::vector<DM::Node*> roof = TBVectorData::getNodeListFromFace(city, f);
                std::reverse(roof.begin(), roof.end());
                f = city->addFace(roof, geometryView);*/

                f->addAttribute("type", "ceiling_roof");

                f->getAttribute("color")->setDoubleVector(roofColor);
            }
            BuildingInterface->getAttribute("Geometry")->setLink("Geometry", f->getUUID());
            f->getAttribute("Parent")->setLink(buildingView.getName(), BuildingInterface->getUUID());

        }
    }



}

void LittleGeometryHelpers::CreateRoofRectangle(DM::System *city, DM::View & buildingView, DM::View &geometryView, DM::Component *BuildingInterface, std::vector<DM::Node * >  &footprint, double height, double alpha)
{
    std::vector<double> roofColor;
    roofColor.push_back(178./255.);
    roofColor.push_back(34./255.);
    roofColor.push_back(34./255.);
    std::vector<double> wallColor;
    wallColor.push_back(196./255.);
    wallColor.push_back(196./255.);
    wallColor.push_back(196./255.);
    std::vector<double> windowColor;
    windowColor.push_back(204./255.);
    windowColor.push_back(229./255.);
    windowColor.push_back(1);

    const double pi =  3.14159265358979323846;
    //Create roof for minimal bounding box
    std::vector<DM::Node> b_box;
    std::vector<double> dimension;

    double angle = DM::CGALGeometry::CalculateMinBoundingBox(footprint, b_box, dimension);
    DM::Face * foot = city->addFace(footprint);
    DM::Node center = TBVectorData::CaclulateCentroid(city, foot);

    QTransform t;
    t.rotate(angle);

    double l = dimension[0];
    double b = dimension[1];


    double h = cos (alpha / 180. * pi) * b / 2.;
    QPointF qcenter(center.getX(), center.getY());
    QPointF q_top_1 = t.map(QPointF (  -l/2., 0)) + qcenter;
    QPointF q_top_2 =  t.map(QPointF (l/2.,  0)) + qcenter;
    /**
      * 4--------3
      * |        |
      * t1-------t2
      * |        |
      * 1--------2
      */

    QPointF q_n1 =  t.map(QPointF (  -l/2.,  b/2.)) + qcenter;
    QPointF q_n2 =  t.map(QPointF (  l/2., b/2.)) + qcenter;
    QPointF q_n3 =  t.map(QPointF (  l/2.,  -b/2.)) + qcenter;
    QPointF q_n4 =  t.map(QPointF (  -l/2.,  -b/2.)) + qcenter;

    DM::Node * n4 = city->addNode(q_n1.x(), q_n1.y(), height);
    DM::Node * n3 = city->addNode(q_n2.x(), q_n2.y(), height);
    DM::Node * n2 = city->addNode(q_n3.x(), q_n3.y(), height);
    DM::Node * n1 = city->addNode(q_n4.x(), q_n4.y(), height);

    DM::Node * t1 = city->addNode(q_top_1.x(), q_top_1.y(), height + h/2.);
    DM::Node * t2 = city->addNode(q_top_2.x(), q_top_2.y(), height + h/2.);

    std::vector<DM::Node* > vF1;
    vF1.push_back(n1);
    vF1.push_back(n2);
    vF1.push_back(t2);
    vF1.push_back(t1);
    vF1.push_back(n1);

    DM::Face * F1 =  city->addFace(vF1, geometryView);
    F1->getAttribute("Parent")->setLink(buildingView.getName(), BuildingInterface->getUUID());
    F1->addAttribute("type", "roof");
    F1->getAttribute("color")->setDoubleVector(roofColor);
    BuildingInterface->getAttribute("Geometry")->setLink("Geometry", F1->getUUID());


    std::vector<DM::Node* > vF2;
    vF2.push_back(t1);
    vF2.push_back(t2);
    vF2.push_back(n3);
    vF2.push_back(n4);
    vF2.push_back(t1);

    DM::Face * F2 =  city->addFace(vF2, geometryView);
    F2->getAttribute("Parent")->setLink(buildingView.getName(), BuildingInterface->getUUID());
    F2->addAttribute("type", "roof");
    F2->getAttribute("color")->setDoubleVector(roofColor);
    BuildingInterface->getAttribute("Geometry")->setLink("Geometry", F2->getUUID());

    std::vector<DM::Node* > vF3;
    vF3.push_back(n1);
    vF3.push_back(t1);
    vF3.push_back(n4);
    vF3.push_back(n1);

    DM::Face * F3 =  city->addFace(vF3, geometryView);
    F3->getAttribute("Parent")->setLink(buildingView.getName(), BuildingInterface->getUUID());
    F3->addAttribute("type", "roof_wall");
    F3->getAttribute("color")->setDoubleVector(wallColor);
    BuildingInterface->getAttribute("Geometry")->setLink("Geometry", F3->getUUID());

    std::vector<DM::Node* > vF4;
    vF4.push_back(n2);
    vF4.push_back(n3);
    vF4.push_back(t2);
    vF4.push_back(n2);

    DM::Face * F4 =  city->addFace(vF4, geometryView);
    F4->getAttribute("Parent")->setLink(buildingView.getName(), BuildingInterface->getUUID());
    F4->addAttribute("type", "roof_wall");
    F4->getAttribute("color")->setDoubleVector(wallColor);
    BuildingInterface->getAttribute("Geometry")->setLink("Geometry", F4->getUUID());
}
