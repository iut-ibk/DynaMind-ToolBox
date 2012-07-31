#include "dmviewer.h"

#include "dmcomponent.h"
#include "dmsystem.h"
#include "dmnode.h"
#include "dmface.h"
#include "dmedge.h"
#include "dmattribute.h"

#include <QDebug>
#include <QGLViewer/vec.h>

#include <string>
#include <limits>
#include <algorithm>


#define CHECK_SYSTEM if (!system) return

namespace DM {

void Viewer::init() {
    CHECK_SYSTEM;
    glDisable(GL_CULL_FACE);
}

void Viewer::drawWithNames() {
    CHECK_SYSTEM;
}

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
        
        Node *n = system->getNode(e->getStartpointName());
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

template<int SD_GL_PRIMITIVE>
struct SimpleDrawer {
    void operator()(DM::System *s, DM::View v, void *f_e, DM::Node *n, iterator_pos pos) {
        if (pos == before) {
            glBegin(SD_GL_PRIMITIVE);
            return;
        }
        if (pos == after) {
            glEnd();
            return;
        }
        glVertex3dv(n->get());
    }
};

struct Drawer {
    void operator()(DM::System *s, DM::View v, DM::Face *f, DM::Node *n, iterator_pos pos) {
        if (pos == before) {
            glBegin(GL_LINE_LOOP);
            return;
        }
        if (pos == after) {
            glEnd();
            return;
        }
        glVertex3dv(n->get());
    }
    
    void operator()(DM::System *s, DM::View v, DM::Edge *e, DM::Node *n, iterator_pos pos) {
        if (pos == before) {
            glBegin(GL_LINES);
            return;
        }
        if (pos == after) {
            glEnd();
            return;
        }
        glVertex3dv(n->get());
    }
};

void Viewer::draw() {
    CHECK_SYSTEM;
    if (!glIsList(list)) {
        list = glGenLists(1);
        glNewList(list, GL_COMPILE_AND_EXECUTE);
        Drawer drawer;
        foreach(DM::View v, system->getViews()) {
            if (v.getType() == DM::FACE) {
                SimpleDrawer<GL_POLYGON> sd;
                iterate_faces(system, v, sd);
            }
            if (v.getType() == DM::EDGE) {
                SimpleDrawer<GL_LINES> sd;
                iterate_edges(system, v, sd);
            }
        }
        glEndList();
    } else {
        glCallList(list);
    }
}

void Viewer::postSelection(const QPoint &point) {
    CHECK_SYSTEM;
}

struct FindBounds {
    double min[3];
    double max[3];
    QMap<std::string, double> attrs_min;
    QMap<std::string, double> attrs_max;
    
    void max_vec(const double *other) {
        max[0] = std::max(other[0], max[0]);
        max[1] = std::max(other[1], max[1]);
        max[2] = std::max(other[2], max[2]);
    }
    
    void min_vec(const double *other) {
        min[0] = std::min(other[0], min[0]);
        min[1] = std::min(other[1], min[1]);
        min[2] = std::min(other[2], min[2]);
    }
    
    FindBounds() {
        min[0] = std::numeric_limits<double>::max();
        min[2] = min[1] = min[0];
        max[0] = std::numeric_limits<double>::min();
        max[2] = max[1] = max[0];
    }
    
    void operator()(DM::System *, DM::View , DM::Face *f, DM::Node *n, iterator_pos pos) {
        if (pos == before) {
            QMap<std::string, DM::Attribute*> attrs(f->getAllAttributes());
            int n = 0;
        }
        if (pos != in_between) return;
        min_vec(n->get());
        max_vec(n->get());
    }
    void operator()(DM::System *s, DM::View v, DM::Edge *f, DM::Node *n, iterator_pos pos) {
        if (pos != in_between) return;
        min_vec(n->get());
        max_vec(n->get());
    }
    
    double radius() {
        double span[3] = {max[0] - min[0], 
                          max[1] - min[1], 
                          0 /*max[2] - min[2]*/};
        return std::max(span[0], span[1])/2.0;
    }
};

void Viewer::systemChanged() {
    FindBounds fb;
    foreach(DM::View v, system->getViews()) {
        if (v.getType() == DM::FACE) {
            iterate_faces(system, v, fb);
        }
    }
    
    setSceneRadius(fb.radius());
    using namespace qglviewer;
    
    Vec c(Vec(fb.min) + Vec(fb.radius(), fb.radius(), 0));
    setSceneCenter(c);
    showEntireScene();
    
    if (glIsList(list)) {
        glDeleteLists(list, 1);
    }
}

}
