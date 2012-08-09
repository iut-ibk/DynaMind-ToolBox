#ifndef DM_DMLAYER_H
#define DM_DMLAYER_H

#include "dmview.h"

#include <QMap>
#include <GL/gl.h>

#include "dmviewmetadata.h"

namespace DM {

class System;
class Attribute;
class Face;
class Node;

class Layer {
public:
    Layer(System *system, View v, const std::string &attribute = 0);
    ~Layer();
    
    void setColorInterpretation(GLuint texture);
    void setHeightInterpretation(float percent);
    
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
