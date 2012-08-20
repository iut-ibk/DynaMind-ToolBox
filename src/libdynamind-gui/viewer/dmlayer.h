#ifndef DM_DMLAYER_H
#define DM_DMLAYER_H

#include "dmview.h"

#include <QMap>
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>

#include "dmviewmetadata.h"

namespace DM {

class System;
class Attribute;
class Face;
class Node;

class Layer {
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
    
    void draw();
    void drawWithNames();
    
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
        return name >= 0 && name < vmd.number_of_primitives;
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
    
private:
    System *system;
    View view;
    std::string attribute;
    double scale_height;
    
    std::vector<GLuint> lists;
    int texture;
    GLuint name_start;
    
    double x_off, y_off, z_off;
    
    ViewMetaData vmd;
    
    int attribute_vector_name;
};

} // namespace DM

#endif // DM_DMLAYER_H
