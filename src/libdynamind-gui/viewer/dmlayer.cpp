#include "dmlayer.h"

#include "dmcomponent.h"
#include "dmnode.h"
#include "dmface.h"
#include "dmedge.h"
#include "dmattribute.h"
#include "dmsystemiterators.h"

#ifdef _WIN32
#include <windows.h>
typedef GLvoid (__stdcall *_GLUfuncptr)(GLvoid);
#endif
#include <GL/glu.h>

#include <QImage>
#include <QPainter>
#include <cassert>

namespace DM {

template<int SD_GL_PRIMITIVE>
struct SimpleDrawer {

    void operator()(DM::System *s, DM::View v, void *f_e, DM::Node *n, iterator_pos pos) {
        if (pos == before) {
            glBegin(SD_GL_PRIMITIVE);
            return;
        }
        if (pos == after) {
            glEnd();
            return;
        }
        glColor3f(0, 0, 0);
        glVertex3dv(n->get());
    }
};

void error_callback(GLenum e) {
    const char *error_string = (const char *) gluErrorString(e);
    std::cout << "error while tesselating: " << error_string << std::endl;
}

void color_callback(GLdouble *d, void *data) {
    glColor3f(0.0, 0.0, 0.0);
    glVertex3dv(d);
}

void texture_callback(GLdouble *d, void *data) {
    glColor3f(1, 1, 1);
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
    double height_scale;
    double attr_span;
    const Layer &l;
    std::vector<GLdouble *> vertices;
    int name_start;
    
    TesselatedFaceDrawer(const Layer &l)
        : l(l), height_scale(0.0), name_start(l.getNameStart()) {
        if (l.getAttribute() == "") {
            return;
        }
        if (l.getHeightInterpretation() > 0.0) {
            const ViewMetaData &vmd = l.getViewMetaData();
            this->height_scale = 1.0/vmd.attr_max*vmd.radius() * l.getHeightInterpretation();
            this->attr_span = vmd.attr_max - vmd.attr_min;
        }
        
    }
    
    void operator()(DM::System *s, DM::View v, DM::Face *f, DM::Node *n, iterator_pos pos) {
        if (pos == after) {
            render();
            name_start++;
            return;
        }
        if (pos != in_between) return;
        
        GLdouble *d = new GLdouble[5];
        
        d[0] = n->getX();
        d[1] = n->getY();
        d[2] = 0.0;
        if (height_scale > 0) {
            Attribute *a = f->getAttribute(l.getAttribute());
            if (a->getType() == Attribute::DOUBLE) {
                d[2] = f->getAttribute(l.getAttribute())->getDouble() * height_scale;
            } else {
                double attr_value = a->getDoubleVector()[l.getAttributeVectorName()];
                d[2] = attr_value * height_scale;
            }
        }
        
        if (glIsTexture(l.getColorInterpretation())) {
            const ViewMetaData &vmd = l.getViewMetaData();
            Attribute *a = f->getAttribute(l.getAttribute());
            if (a->getType() == Attribute::DOUBLE) {
                d[3] = (a->getDouble() - vmd.attr_min) / attr_span;
            } else {
                d[3] = (a->getDoubleVector()[l.getAttributeVectorName()] - vmd.attr_min) / attr_span;
            }
            d[4] = 0.5;
        } else {
            d[3] = d[4] = 0.0;
        }
        vertices.push_back(d);
    }
    
    void render() {
        if (glIsTexture(l.getColorInterpretation())) {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, l.getColorInterpretation());
        }
        assert(glGetError() == GL_NO_ERROR);
        GLUtesselator *tess = gluNewTess();
        gluTessCallback(tess, GLU_TESS_ERROR, (_GLUfuncptr) error_callback);
        gluTessCallback(tess, GLU_TESS_BEGIN, (_GLUfuncptr) glBegin);
        gluTessCallback(tess, GLU_TESS_COMBINE_DATA, (_GLUfuncptr) combine_callback);
        if (glIsTexture(l.getColorInterpretation())) {
            gluTessCallback(tess, GLU_TESS_VERTEX, (_GLUfuncptr) texture_callback);
        } else {
            gluTessCallback(tess, GLU_TESS_VERTEX, (_GLUfuncptr) color_callback);
        }
        gluTessCallback(tess, GLU_TESS_END, glEnd);
        
        glPushName(name_start);
        
        GLuint texture = l.getColorInterpretation();
        
        gluTessBeginPolygon(tess, &texture);
        gluTessBeginContour(tess);
        
        foreach(GLdouble *v, vertices) {
            gluTessVertex(tess, v, v);
        }
        
        gluTessEndContour(tess);
        gluTessEndPolygon(tess);
        gluDeleteTess(tess);
        
        glPopName();
        
        assert(glGetError() == GL_NO_ERROR);
        
        foreach (GLdouble *v, vertices) {
            delete[] v;
        }
        
        vertices.clear();
        
        if (glIsTexture(texture)) glDisable(GL_TEXTURE_2D);
    }
};


Layer::Layer(System *s, View v, const std::string &a) 
    : system(s), view(v), 
      attribute(a), vmd(a),
      attribute_vector_name(0), texture(-1),
      scale_height(-1) {
}

void Layer::draw() {
    if (lists.size() <= attribute_vector_name) {
        lists.resize(attribute_vector_name+1, -1);
    }
    if (!glIsList(lists[attribute_vector_name])) {
        lists[attribute_vector_name] = glGenLists(1);
        glNewList(lists[attribute_vector_name], GL_COMPILE);
        
        if (view.getType() == DM::FACE) {
            TesselatedFaceDrawer drawer(*this);
            iterate_faces(system, view, drawer);
        }
        if (view.getType() == DM::EDGE) {
            SimpleDrawer<GL_LINES> drawer;
            iterate_edges(system, view, drawer);
        }
        if (view.getType() == DM::NODE) {
            SimpleDrawer<GL_POINTS> drawer;
            iterate_nodes(system, view, drawer);
        }
        
        glEndList();
    }
    
    glPushMatrix();
    glTranslated(x_off, y_off, z_off);
    assert(glIsList(lists[attribute_vector_name]));
    glCallList(lists[attribute_vector_name]);
    glPopMatrix();
}

void Layer::drawWithNames() {
    draw();
}

void Layer::systemChanged() {
    vmd = ViewMetaData(attribute);

    if (view.getType() == DM::FACE) {
        iterate_faces(system, view, vmd);
    }
    if (view.getType() == DM::EDGE) {
        iterate_edges(system, view, vmd);
    }
    if (view.getType() == DM::NODE) {
        iterate_nodes(system, view, vmd);
    }
    
    foreach (GLuint list, lists) {
        if (glIsList(list)) {
            glDeleteLists(list, 1);
        }
    }
}

} // namespace DM
