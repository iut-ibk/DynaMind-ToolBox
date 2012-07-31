#ifndef DM3DVIEWER_H
#define DM3DVIEWER_H

#include <QGLViewer/qglviewer.h>

namespace DM {
class System;

class Viewer : public QGLViewer {
public:
    Viewer(QWidget *parent = 0) : QGLViewer(parent), system(0) {
    }

    void setSystem(System *system) {
        this->system = system;
        systemChanged();
    }
    
public slots:
    void systemChanged();
    
protected:
    virtual void init();
    virtual void drawWithNames();
    virtual void draw();
    virtual void postSelection(const QPoint &point);
    
private:
    System *system;
    int list;
};

}
#endif // DM3DVIEWER_H
