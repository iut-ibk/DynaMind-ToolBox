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
    void drawWithNames(int start_name);
    
    void setOffset(double x, double y, double z) {
        x_off = x;
        y_off = y;
        z_off = z;
    }
    
private:
    System *system;
    View view;
    std::string attribute;
    double scale_height;
    
    int list;
    int texture;
    
    double x_off, y_off, z_off;
    
    ViewMetaData vmd;
};

} // namespace DM

#endif // DM_DMLAYER_H
