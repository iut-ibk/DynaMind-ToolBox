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

#ifndef DM3DVIEWER_H
#define DM3DVIEWER_H

#include <QGLViewer/qglviewer.h>

namespace DM {
class System;
class Layer;

class Viewer : public QGLViewer {
public:
    Viewer(QWidget *parent = 0) : QGLViewer(parent), system(0) {
    }

    void setSystem(System *system) {
        this->system = system;
        systemChanged();
    }
    
    void addLayer(Layer *l);
    
public slots:
    void systemChanged();
    void updateLayerLayout();
    
    void setLayerOffset(double x, 
                        double y,
                        double z) {
        x_off = x;
        y_off = y;
        z_off = z;
        updateLayerLayout();
    }
    
    void setAttributeVectorName(int name);
    
protected:
    virtual void init();
    virtual void drawWithNames();
    virtual void draw();
    virtual void postDraw();
    virtual void postSelection(const QPoint &point);
    
private:
    void drawColorBars();
    
    System *system;
    int list;
    std::vector<Layer*> layers;
    double x_off, y_off, z_off;
    double max_radius;
    int max_layer_index;
};

}
#endif // DM3DVIEWER_H
