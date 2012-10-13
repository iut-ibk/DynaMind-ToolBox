#include "dmcolorbarwidget.h"
#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#ifdef _WIN32
  #include <windows.h>
#endif
#include <GL/glu.h>
#endif

namespace DM {

ColorBarWidget::ColorBarWidget(QWidget *parent) :
    QGLWidget(parent) {
}

ColorBarWidget::ColorBarWidget(QWidget *parent, QGLWidget *shared) :
    QGLWidget(parent, shared)  {
}

void ColorBarWidget::initializeGL() {
    glClearColor(0, 0.0, 0, 1.0);
}

void ColorBarWidget::paintGL() {
    //glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_TEXTURE_1D);
    if (!glIsTexture(texture)) {
        return;
    }
    glBindTexture(GL_TEXTURE_1D, texture);
    
    glBegin(GL_QUADS);
        glColor3f(1.0, 1.0, 1.0);
        
        glTexCoord1f(0.0);
        glVertex2f(0, height());
        
        glTexCoord1f(1.0);
        glVertex2f(width(), height());
    
        glTexCoord1f(1.0);
        glVertex2f(width(), 0);
        
        glTexCoord1f(0.0);
        glVertex2f(0, 0);
    glEnd();
    
    glDisable(GL_TEXTURE_1D);
}

void ColorBarWidget::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
}

} // namespace DM
