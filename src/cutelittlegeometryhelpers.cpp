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



std::vector<DM::Face*> CuteLittleGeometryHelpers::CreateHolesInAWall(DM::System *sys, DM::Face *f, double distance, double width, double height)
{
    std::vector<DM::Node*> nodes = TBVectorData::getNodeListFromFace(sys, f);
    //And again we rotate or wall again into x,y assuming. we assume that z is becoming our new x
    double E[3][3];
    TBVectorData::CorrdinateSystem( DM::Node(0,0,0), DM::Node(1,0,0), DM::Node(0,1,0), E);

    double E_to[3][3];

    TBVectorData::CorrdinateSystem( *(nodes[0]), *(nodes[1]), *(nodes[2]), E_to);

    double alphas[3][3];
    TBVectorData::RotationMatrix(E, E_to, alphas);


    DM::System transformedSys;

    std::vector<DM::Node*> ns_t;
    double z_const = 0;
    for (int i = 0; i < nodes.size(); i++) {
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

    double true_distance = l / numberOfSegments;

    std::vector<DM::Node> window_nodes;

    //We will distribute the windows equally as parapet height we asume 0.80 m
    for (int i = 0; i < numberOfSegments; i++) {
        //Center of the new window
        double delta_y = l / (numberOfSegments+1) * (i+1) + ymin;

        DM::Node n1(xmin + 1.2, delta_y - width/2, z_const);
        DM::Node n2(xmin + 1.2 + height,  delta_y - width/2, z_const);
        DM::Node n3(xmin + 1.2 + height,  delta_y + width/2, z_const);
        DM::Node n4(xmin + 1.2,  delta_y + width/2, z_const);

        window_nodes.push_back(n1);
        window_nodes.push_back(n2);
        window_nodes.push_back(n3);
        window_nodes.push_back(n4);

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

        windows.push_back(sys->addFace(window_nodes_t));

        f->addHole(window_nodes_t);

        DM::Logger(DM::Debug) << "Add Window";
    }
    return windows;
}
