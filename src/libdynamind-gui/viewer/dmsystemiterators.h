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
