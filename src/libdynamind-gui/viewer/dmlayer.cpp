#include "dmlayer.h"

#include "dmcomponent.h"
#include "dmnode.h"
#include "dmface.h"
#include "dmedge.h"
#include "dmattribute.h"
#include "dmsystemiterators.h"

#include <GL/gl.h>
#include <GL/glu.h>

#include <QImage>
#include <QPainter>
#include <cassert>

namespace DM {

template<int SD_GL_PRIMITIVE>
struct SimpleDrawer {
    
    SimpleDrawer() {
        
    }
    
    void operator()(DM::System *s, DM::View v, void *f_e, DM::Node *n, iterator_pos pos) {
        if (pos == before) {
            glBegin(SD_GL_PRIMITIVE);
            return;
        }
        if (pos == after) {
            glEnd();
            return;
        }
        glVertex3dv(n->get());
    }
};

void error_callback(GLenum e) {
    const char *error_string = (const char *) gluErrorString(e);
    int x = 100;
}

void color_callback(GLdouble *d, void *data) {
    glColor3f(1.0, 1.0, 1.0);
    glVertex3dv(d);
}

void texture_callback(GLdouble *d, void *data) {
    glTexCoord2dv(&d[3]);
    glVertex3dv(d);
}

void combine_callback(GLdouble coords[3], 
                      GLdouble *vertex_data[4],
                      GLfloat weight[4], GLdouble **dataOut,
                      void *user_data) {
    GLuint *texture = (GLuint*) user_data;
    GLdouble *vertex = new GLdouble[5]; //LEAAAAAK!!!
    vertex[0] = coords[0];
    vertex[1] = coords[1];
    vertex[2] = coords[2];
    
    GLdouble *v1 = vertex_data[0];
    
    if (glIsTexture(*texture)) {
        vertex[3] = v1[3];
        vertex[4] = v1[4];
    }
    
    *dataOut = vertex;
}

struct TesselatedFaceDrawer {
    std::string attr;
    double height_scale, attr_span;
    GLuint texture;
    const ViewMetaData &vmd;
    
    std::vector<GLdouble *> vertices;
    
    TesselatedFaceDrawer(const ViewMetaData &vmd, std::string attr = "", double height_scale = -1, GLuint texture = -1)
        : attr(attr), texture(texture), vmd(vmd) {
        
        this->height_scale = 1.0/vmd.attr_max*vmd.radius()*height_scale;
        this->attr_span = vmd.attr_max - vmd.attr_min;
    }
    
    void operator()(DM::System *s, DM::View v, DM::Face *f, DM::Node *n, iterator_pos pos) {
        if (pos == after) {
            render();
            return;
        }
        if (pos != in_between) return;
        
        GLdouble *d = new GLdouble[5];
        
        d[0] = n->getX();
        d[1] = n->getY();
        d[2] = height_scale > 0 ? f->getAttribute(attr)->getDouble() * height_scale : 0.0;
        if (glIsTexture(texture)) {
            d[3] = (f->getAttribute(attr)->getDouble() - vmd.attr_min) / attr_span;
            d[4] = 0.5;
        } else {
            d[3] = d[4] = 0.0;
        }
        vertices.push_back(d);
    }
    
    void render() {
        if (glIsTexture(texture)) {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, texture);
        }
        assert(glGetError() == GL_NO_ERROR);
        GLUtesselator *tess = gluNewTess();
        gluTessCallback(tess, GLU_TESS_ERROR, (_GLUfuncptr) error_callback);
        gluTessCallback(tess, GLU_TESS_BEGIN, (_GLUfuncptr) glBegin);
        gluTessCallback(tess, GLU_TESS_COMBINE_DATA, (_GLUfuncptr) combine_callback);
        if (glIsTexture(texture)) {
            gluTessCallback(tess, GLU_TESS_VERTEX, (_GLUfuncptr) texture_callback);
        } else {
            gluTessCallback(tess, GLU_TESS_VERTEX, (_GLUfuncptr) color_callback);
        }
        gluTessCallback(tess, GLU_TESS_END, glEnd);
        
        
        gluTessBeginPolygon(tess, &this->texture);
        gluTessBeginContour(tess);
        
        foreach(GLdouble *v, vertices) {
            gluTessVertex(tess, v, v);
        }
        
        gluTessEndContour(tess);
        gluTessEndPolygon(tess);
        gluDeleteTess(tess);
        
        assert(glGetError() == GL_NO_ERROR);
        
        foreach (GLdouble *v, vertices) {
            delete v;
        }
        
        vertices.clear();
        
        if (glIsTexture(texture)) glDisable(GL_TEXTURE_2D);
    }
};


Layer::Layer(System *s, View v, const std::string &a) 
    : system(s), 
      view(v), 
      attribute(a), vmd(a) {
}

void Layer::setColorInterpretation(GLuint texture) {
    this->texture = texture;
}

void Layer::setHeightInterpretation(float percent) {
    scale_height = percent;
}

void Layer::draw() {
    if (!glIsList(list)) {
        list = glGenLists(1);
        glNewList(list, GL_COMPILE);
        
        if (view.getType() == DM::FACE) {
            TesselatedFaceDrawer drawer(vmd, attribute, scale_height, texture);
            iterate_faces(system, view, drawer);
        }
        if (view.getType() == DM::EDGE) {
            SimpleDrawer<GL_LINES> drawer;
            iterate_edges(system, view, drawer);
        }
        
        glEndList();
    }
    
    glPushMatrix();
    glTranslated(x_off, y_off, z_off);
    glCallList(list);
    glPopMatrix();
}

void Layer::drawWithNames(int start_name) {
}

void Layer::systemChanged() {
    vmd = ViewMetaData(attribute);

    if (view.getType() == DM::FACE) {
        iterate_faces(system, view, vmd);
    }
    if (view.getType() == DM::EDGE) {
        iterate_edges(system, view, vmd);
    }
    
    if (glIsList(list)) {
        glDeleteLists(list, 1);
    }
}

} // namespace DM
