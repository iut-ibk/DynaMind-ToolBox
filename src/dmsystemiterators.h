/**
 * @file
 * @author  Gregor Burger <burger.gregor@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2012  Gregor Burger
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#ifndef SYSTEM_ITERATORS_H
#define SYSTEM_ITERATORS_H

#include "dmsystem.h"
#include "dmview.h"
#include "dmnode.h"
#include "dmedge.h"
#include "dmface.h"
#include "dmcomponent.h"
#include "dmattribute.h"
#include "tbvectordata.h"
#include "cgalgeometry.h"
#include "triangulaterasterdata.h"

#include <dmlogger.h>

#include <QtGlobal>

enum iterator_pos {
    before,
    after,
    in_between
};

template<typename CB>
void iterate_components(DM::System *system, DM::View v, CB &callback = CB()) {
    foreach(std::string cmp_uuid, system->getUUIDsOfComponentsInView(v)) {

        DM::Component *cmp = system->getComponent(cmp_uuid);
        callback(system, v, cmp, 0, 0, before);
        std::vector<DM::LinkAttribute> links = cmp->getAttribute("Geometry")->getLinks();
        std::vector<std::string> uuids;
        foreach (DM::LinkAttribute link, links) {
            uuids.push_back(link.uuid);
        }
        if (v.getType() == DM::FACE)
            uuids.push_back(cmp_uuid);
        foreach (std::string uuid, uuids) {
            DM::Face * f = system->getFace(uuid);
            std::vector<double> c = f->getAttribute("color")->getDoubleVector();
            int size_c = c.size();
            DM::Vector3 color;
            if (size_c > 2) {
				color = DM::Vector3(c[0],c[1],c[2]);
                /*color.setX(c[0]);
                color.setY(c[1]);
                color.setZ(c[2]);*/
            }
            std::vector<DM::Node> nodes = DM::CGALGeometry::FaceTriangulation(system, f);
            foreach (DM::Node n, nodes) 
			{
                /*n.addAttribute("r", color.getX());
                n.addAttribute("g", color.getY());
                n.addAttribute("b", color.getZ());*/
				
				DM::Vector3 vec;
				n.get(&vec.x);
                callback(system, v, cmp, &vec, &color, in_between);
            }
        }
        callback(system, v, cmp, 0, 0, after);
    }
}

template<typename CB>
void iterate_mesh(DM::System *system, DM::View v, CB &callback = CB()) {

    foreach(std::string cmp_uuid, system->getUUIDsOfComponentsInView(v)) {

        DM::Face *f = system->getFace(cmp_uuid);

        callback(system, v, f, 0, 0, before);

		foreach(DM::Node* n, f->getNodePointers())
		{
			DM::Vector3 vec;
			n->get(&vec.x);
			callback(system, v, n, &vec, 0, in_between);
		}

        /*std::vector<std::string> uuids_nodes = f->getNodes();

        for (int i = 0; i < 3; i++) {
            DM::Node * n = system->getNode(uuids_nodes[i]);
            callback(system, v, n, n, in_between);
        }*/
        callback(system, v, f, 0, 0, after);
    }
}


template<typename CB>
void iterate_nodes(DM::System *system, DM::View v, CB &callback = CB()) {

    foreach(std::string node_uuid, system->getUUIDsOfComponentsInView(v)) {
        DM::Node *n = system->getNode(node_uuid);
		DM::Vector3 vec;
		n->get(&vec.x);

        callback(system, v, n, 0, 0, before);
        callback(system, v, n, &vec, 0, in_between);
        callback(system, v, n, 0, 0, after);
    }

}

template<typename CB>
void iterate_rasterdata(DM::System *system, DM::View v, CB &callback = CB()) {
    DM::ComponentMap cmp = system->getAllComponentsInView(v);
    DM::RasterData * r = 0;
    for (DM::ComponentMap::const_iterator it = cmp.begin();
         it != cmp.end();
         ++it) {
        r = (DM::RasterData *) it->second;
    }

    callback(system, v, r, 0, 0, before);

    std::vector<DM::Vector3> points;
    TriangulateRasterData::Triangulation(points, r);

	for (unsigned int i = 0; i < points.size(); i++)
		callback(system, v, r, &points[i], 0, in_between);

    /*for (unsigned int i = 0; i < nodes.size(); i++) {
        callback(system, v, r, &nodes[i], in_between);
    }*/

    callback(system, v, r, 0, 0, after);
}

template<typename CB>
void iterate_edges(DM::System *system, DM::View v, CB &callback = CB()) {
    foreach(std::string edge_uuid, system->getUUIDsOfComponentsInView(v)) {
        DM::Edge *e = system->getEdge(edge_uuid);

        callback(system, v, e, 0, 0, before);

        DM::Node *n = system->getNode(e->getStartpointName());
		DM::Vector3 vec;
		n->get(&vec.x);
        callback(system, v, e, &vec, 0, in_between);

        n = system->getNode(e->getEndpointName());
		n->get(&vec.x);
        callback(system, v, e, &vec, 0, in_between);

        callback(system, v, e, 0, 0, after);
    }
}

template<typename CB>
void iterate_faces(DM::System *system, DM::View v, CB &callback = CB()) {
    foreach(std::string face_uuid, system->getUUIDsOfComponentsInView(v)) {
        DM::Face *f = system->getFace(face_uuid);

        std::vector<std::string> nodes = f->getNodes();
        nodes.pop_back();

        callback(system, v, f, 0, 0, before);

        foreach(std::string node_uuid, nodes) {
            DM::Node *n = system->getNode(node_uuid);
		DM::Vector3 vec;
			n->get(&vec.x);
            callback(system, v, f, &vec, 0, in_between);
            //callback(system, v, f, n, in_between);
        }

        callback(system, v, f, 0, 0, after);
    }
}


#endif // SYSTEM_ITERATORS_H
