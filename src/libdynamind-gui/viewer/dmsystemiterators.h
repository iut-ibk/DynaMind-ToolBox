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
#include <QtGlobal>

enum iterator_pos {
    before,
    after,
    in_between
};

template<typename CB> 
void iterate_nodes(DM::System *system, DM::View v, CB &callback = CB()) {
    
    foreach(std::string node_uuid, system->getUUIDsOfComponentsInView(v)) {
        DM::Node *n = system->getNode(node_uuid);
        callback(system, v, n, 0, before);
        callback(system, v, n, n, in_between);
        callback(system, v, n, 0, after);
    }
    
}

template<typename CB> 
void iterate_edges(DM::System *system, DM::View v, CB &callback = CB()) {
    foreach(std::string edge_uuid, system->getUUIDsOfComponentsInView(v)) {
        DM::Edge *e = system->getEdge(edge_uuid);
        
        callback(system, v, e, 0, before);
        
        DM::Node *n = system->getNode(e->getStartpointName());
        callback(system, v, e, n, in_between);
        
        n = system->getNode(e->getEndpointName());
        callback(system, v, e, n, in_between);
        
        callback(system, v, e, 0, after);
    }
}

template<typename CB> 
void iterate_faces(DM::System *system, DM::View v, CB &callback = CB()) {
    foreach(std::string face_uuid, system->getUUIDsOfComponentsInView(v)) {
        DM::Face *f = system->getFace(face_uuid);
        std::vector<std::string> nodes = f->getNodes();
        nodes.pop_back();
        
        callback(system, v, f, 0, before);
        
        foreach(std::string node_uuid, nodes) {
            DM::Node *n = system->getNode(node_uuid);
            callback(system, v, f, n, in_between);
        }
        
        callback(system, v, f, 0, after);
    }
}

#endif // SYSTEM_ITERATORS_H
