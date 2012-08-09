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
    virtual void postSelection(const QPoint &point);
    
private:
    System *system;
    int list;
    std::vector<Layer*> layers;
    double x_off, y_off, z_off;
    double max_radius;
    int max_layer_index;
};

}
#endif // DM3DVIEWER_H
