#ifndef DM_DMCOLORBARWIDGET_H
#define DM_DMCOLORBARWIDGET_H

#include <QGLWidget>

namespace DM {

class ColorBarWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit ColorBarWidget(QWidget *parent = 0);
    ColorBarWidget(QWidget *parent, QGLWidget *shared);
    void setTexture(GLuint texture) {
        this->texture = texture;
        updateGL();
    }
    
private:
    virtual void initializeGL();
    virtual void paintGL();
    virtual void resizeGL(int w, int h);
    
    GLuint texture;
};

} // namespace DM

#endif // DM_DMCOLORBARWIDGET_H
