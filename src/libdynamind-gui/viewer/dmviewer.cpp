#include "dmviewer.h"

#include "dmcomponent.h"
#include "dmsystem.h"
#include "dmnode.h"
#include "dmlayer.h"


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
    foreach(Layer *l, layers) {
        l->drawWithNames(0);
    }
}

void Viewer::draw() {
    CHECK_SYSTEM;
    foreach(Layer *l, layers) {
        l->draw();
    }
}

void Viewer::postSelection(const QPoint &point) {
    CHECK_SYSTEM;
}

void Viewer::addLayer(Layer *l) {
    layers.push_back(l);
    l->systemChanged();
    if (layers.size() == 1) {
        max_layer_index = 0;
        max_radius = l->getViewMetaData().radius();
        updateLayerLayout();
        return;
    }
    
    if (l->getViewMetaData().radius() > max_radius) {
        max_radius = l->getViewMetaData().radius();
        max_layer_index = layers.size() - 1;
    }
    
    updateLayerLayout();
}

void Viewer::systemChanged() {
    foreach(Layer *l, layers) {
        l->systemChanged();
    }
    updateLayerLayout();
}

void Viewer::updateLayerLayout() {
    if (!layers.size()) return;
    
    const ViewMetaData &vmd = layers[max_layer_index]->getViewMetaData();
    
    int i = 0;
    foreach (Layer *l, layers) {
        double x = i * max_radius * x_off;
        double y = i * max_radius * y_off;
        double z = i * max_radius * z_off;
        l->setOffset(x, y, z);
        i++;
    }
    
    setSceneRadius(max_radius);
    using namespace qglviewer;
    Vec c(Vec(vmd.min) + Vec(max_radius, max_radius, 0));
    setSceneCenter(c);
    
    //update camera only the first time
    if (layers.size() == 1) {
        showEntireScene();
    } else {
        updateGL();
    }
}

}
