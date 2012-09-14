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

#ifndef DM_DMLAYER_H
#define DM_DMLAYER_H
#include <dmcompilersettings.h>
#include "dmview.h"

#include <QMap>
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>

#include "dmviewmetadata.h"

class QWidget;

namespace DM {

class System;
class Attribute;
class Face;
class Node;

class DM_HELPER_DLL_EXPORT Layer {
public:
    Layer(System *system, View v, const std::string &attribute = "");
    ~Layer();
    
    void setColorInterpretation(GLuint texture) {
        this->texture = texture;
    }

    GLuint getColorInterpretation() const {
        return texture;
    }
    
    void setHeightInterpretation(float percent) {
        this->scale_height = percent;
    }
    
    float getHeightInterpretation() const {
        return scale_height;
    }
    
    void systemChanged();
    
    const ViewMetaData &getViewMetaData() const {
        return vmd;
    }
    
    void draw(QWidget *parent);
    void drawWithNames(QWidget *parent);
    
    void setOffset(double x, double y, double z) {
        x_off = x;
        y_off = y;
        z_off = z;
    }
    
    void setNameStart(GLuint start) {
        name_start = start;
    }
    
    GLuint getNameStart() const {
        return name_start;
    }
    
    bool isNameFromLayer(GLuint name) const {
        name -= name_start;
        return name >= 0 && name < (GLuint) vmd.number_of_primitives;
    }
    
    View getView() const {
        return view;
    }
    
    void setAttributeVectorName(int name) {
        attribute_vector_name = name;
    }
    
    int getAttributeVectorName() const {
        return attribute_vector_name;
    }
    
    std::string getAttribute() const {
        return attribute;
    }
    
    void setEnabled(bool enabled) {
        this->enabled = enabled;
    }
    
    bool isEnabled() const {
        return enabled;
    }
    
private:
    System *system;
    View view;
    std::string attribute;
    double scale_height;
    
    std::vector<GLuint> lists;
    GLuint texture;
    GLuint name_start;
    
    double x_off, y_off, z_off;
    
    ViewMetaData vmd;
    
    int attribute_vector_name;
    bool enabled;
};

} // namespace DM

#endif // DM_DMLAYER_H
