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

#include "dmviewer.h"

#include "dmcomponent.h"
#include "dmsystem.h"
#include "dmnode.h"
#include "dmlayer.h"
#include "dmcomponenteditor.h"

#include <QGLViewer/vec.h>
#include <QFontMetrics>

#include <string>
#include <limits>
#include <algorithm>


#define CHECK_SYSTEM if (!system) return

namespace DM {

void Viewer::init() {
    CHECK_SYSTEM;
    glDisable(GL_CULL_FACE);
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glPointSize(3);
    glDisable(GL_LIGHTING);
}

void Viewer::drawWithNames() {
    CHECK_SYSTEM;
    foreach(Layer *l, layers) {
        l->drawWithNames(this);
    }
}

#define CBAR_W 200
#define CBAR_H 30

void Viewer::drawColorBars() {
    if (!layers.size())
        return;
    startScreenCoordinatesSystem();
    
    QFont f;
    QFontMetrics fm(f);
    
    for(int i = 0; i < layers.size(); i++) {
        Layer *l = layers[i];
        if (!glIsTexture(l->getColorInterpretation()))
            continue;
        
        int y_off = i*(fm.lineSpacing() + CBAR_H);
        
        double lower = l->getViewMetaData().attr_min;
        double upper = l->getViewMetaData().attr_max;
        
        glColor3f(.0, .0, .0);
        drawText(0, CBAR_H + y_off + fm.lineSpacing() - 1, QString("%1").arg(lower));
        drawText(CBAR_W, CBAR_H + y_off + fm.lineSpacing() - 1, QString("%1").arg(upper));
        
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, l->getColorInterpretation());
        
        glBegin(GL_QUADS);
            glColor3f(1.0, 1.0, 1.0);
            
            
            glTexCoord2f(0.0, 0.0);        
            glVertex2f(0, y_off + CBAR_H);
            
            glTexCoord2f(1.0, 0.0);        
            glVertex2f(CBAR_W, y_off + CBAR_H);
        
            glTexCoord2f(1.0, 1.0);        
            glVertex2f(CBAR_W, y_off);
            
            glTexCoord2f(0.0, 1.0);        
            glVertex2f(0, y_off);
        glEnd();

        glDisable(GL_TEXTURE_2D);
    }
    stopScreenCoordinatesSystem();
}

void Viewer::draw() {
    CHECK_SYSTEM;
    glEnable(GL_MULTISAMPLE);
    foreach(Layer *l, layers) {
        l->draw(this);
    }
    glDisable(GL_MULTISAMPLE);
}

void Viewer::postDraw() {
    drawColorBars();
}

void Viewer::postSelection(const QPoint &) {
    CHECK_SYSTEM;
    int nl = 0;
    int name = selectedName();
    foreach (Layer *l, layers) {
        if (l->isNameFromLayer(selectedName())) {
            View v = l->getView();
            GLuint ns = l->getNameStart();
            std::string uuid = system->getUUIDsOfComponentsInView(v)[name - ns];
            ComponentEditor ce(system->getComponent(uuid));
            ce.exec();
        }
        nl++;
    }
}

void Viewer::addLayer(Layer *l) {
    layers.push_back(l);
    l->systemChanged();
    if (layers.size() == 1) {
        max_layer_index = 0;
        max_radius = l->getViewMetaData().radius();
        l->setNameStart(0);
        updateLayerLayout();
        return;
    }
    
    if (l->getViewMetaData().radius() > max_radius) {
        max_radius = l->getViewMetaData().radius();
        max_layer_index = layers.size() - 1;
    }
    
    Layer *pred = layers[layers.size()-2];
    l->setNameStart(pred->getNameStart() + pred->getViewMetaData().number_of_primitives);
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
    Vec c(Vec(vmd.min) + Vec((vmd.max[0] - vmd.min[0])/2.0, (vmd.max[1] - vmd.min[1])/2.0, 0.0));
    setSceneCenter(c);
    
    //update camera only the first time
    if (layers.size() == 1) {
        showEntireScene();
    } else {
        updateGL();
    }
}

void Viewer::setAttributeVectorName(int name) {
    foreach (Layer *l, layers) {
        l->setAttributeVectorName(name);
    }
    updateGL();
}

}
