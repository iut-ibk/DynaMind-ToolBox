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

#include "dmlayer.h"
#include "dmlogger.h"
#include "dmcomponent.h"
#include "dmnode.h"
#include "dmface.h"
#include "dmedge.h"
#include "dmattribute.h"
#include "dmsystemiterators.h"
#include "tbvectordata.h"

#include <QString>
#include <QStringList>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/partition_2.h>
#include <CGAL/Partition_traits_2.h>
#include <CGAL/Partition_is_valid_traits_2.h>
#include <CGAL/Cartesian.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
//typedef CGAL::Quotient<CGAL::MP_Float>           Number_type;
//typedef CGAL::Lazy_exact_nt<CGAL::Quotient<CGAL::MP_Float> > NT;
//typedef CGAL::Cartesian<NT>             K;

typedef CGAL::Partition_traits_2<K>                         Traits;
typedef CGAL::Is_convex_2<Traits>                           Is_convex_2;
typedef Traits::Point_2                                     Point_2;
typedef Traits::Polygon_2                                   Polygon_2;
typedef std::vector<Polygon_2>                              Polygon_list;
typedef CGAL::Partition_is_valid_traits_2<Traits, Is_convex_2>
Validity_traits;

#include <QImage>
#include <QPainter>
#include <QProgressDialog>
#include <cassert>

namespace DM {

template<int SD_GL_PRIMITIVE>
struct SimpleDrawer {

    GLuint name_start;

    SimpleDrawer(const Layer &l) : name_start(l.getNameStart()) {

    }

    void operator()(DM::System *s, const DM::View& v, void *f_e, DM::Vector3* point, DM::Vector3* color, iterator_pos pos) {
		if(pos == in_between)
			glVertex3dv(&point->x);
		else if (pos == before) 
		{
            glPushName(name_start);
            glBegin(SD_GL_PRIMITIVE);
			glColor3f(0, 0, 0);
        } 
		else if (pos == after) 
		{
            glEnd();
            glPopName();
            name_start++;
        }
    }
};

struct TesselatedFaceDrawer {
    double attr_span;
    const Layer &l;
    double height_scale;
    Polygon_2 polygon;
    double current_height;
    double current_tex;
    int name_start;
    QProgressDialog *dialog;

    TesselatedFaceDrawer(const Layer &l, QWidget *parent)
        : l(l), height_scale(0.0), name_start(l.getNameStart()) 
	{
        dialog = new QProgressDialog("Tesselating Polygons...", "cancel",
                                     0, l.getViewMetaData().number_of_primitives,
                                     parent);
        dialog->show();

        if (l.getAttribute() == "")
            return;

        const ViewMetaData &vmd = l.getViewMetaData();
        this->attr_span = vmd.attr_max - vmd.attr_min;

        if (l.getHeightInterpretation() > 0.0 && this->attr_span != 0.0)
            this->height_scale = 1.0/this->attr_span*vmd.radius() * l.getHeightInterpretation();
    }

    ~TesselatedFaceDrawer() {
        delete dialog;
    }

    void operator()(DM::System *s, const DM::View& v, DM::Face *f, DM::Vector3* point, DM::Vector3* color, iterator_pos pos) {
        if (pos == after) 
		{
            render();
            polygon.clear();
            name_start++;
            current_height = 0.0;
            current_tex = 0.0;
            dialog->setValue(dialog->value()+1);
        }
        else if (pos == before) 
		{
            if (height_scale > 0) 
			{
                Attribute *a = f->getAttribute(l.getAttribute());
                if (a->getType() == Attribute::DOUBLEVECTOR || a->getType() == Attribute::TIMESERIES) 
				{
                    double attr_value = a->getDoubleVector()[l.getAttributeVectorName()];
                    current_height = attr_value * height_scale;
                } 
				else
                    current_height = f->getAttribute(l.getAttribute())->getDouble() * height_scale;
            }
            if (glIsTexture(l.getColorInterpretation())) 
			{
                const ViewMetaData &vmd = l.getViewMetaData();
                Attribute *a = f->getAttribute(l.getAttribute());
                if (a->getType() == Attribute::DOUBLEVECTOR || a->getType() == Attribute::TIMESERIES) 
                    current_tex = (a->getDoubleVector()[l.getAttributeVectorName()] - vmd.attr_min) / attr_span;
                else
                    current_tex = (a->getDouble() - vmd.attr_min) / attr_span;
			}
            else
                current_tex = 0.0;
        }
		else
			polygon.push_back(Point_2(point->x, point->y));
    }

    void render() {
		bool withTexture = glIsTexture(l.getColorInterpretation());
        if (withTexture)
		{
            glEnable(GL_TEXTURE_1D);
            glBindTexture(GL_TEXTURE_1D, l.getColorInterpretation());
        }
        //assert(glGetError() == GL_NO_ERROR);
        if(!polygon.is_simple()) {
            DM::Logger(DM::Error) << "Polygon is not simple can't perform tessilation";
            return;
        }
        if(polygon.size() < 3) {
            DM::Logger(DM::Error) << "Not a polygon";
            return;
        }

        if (polygon.is_clockwise_oriented())
            polygon.reverse_orientation();
        Polygon_list tesselated;
        Validity_traits validity_traits;

       // CGAL::greene_approx_convex_partition_2(polygon.vertices_begin(), polygon.vertices_end(),
                                               //std::back_inserter(tesselated), validity_traits);

        CGAL::approx_convex_partition_2(polygon.vertices_begin(), polygon.vertices_end(),
                                                std::back_inserter(tesselated), validity_traits);

        glPushName(name_start);
        foreach(Polygon_2 poly, tesselated) 
		{
#if 1
            glBegin(GL_POLYGON);
			if(withTexture)
                    glColor4f(1.0, 1.0, 1.0, 0.75);
			else
                    glColor3f(0.0, 0.0, 0.0);


            foreach(Point_2 p, poly.container()) 
			{
                if (withTexture) 
                    glTexCoord1d(current_tex);

                glVertex3d(CGAL::to_double(p.x()), CGAL::to_double(p.y()), current_height);
            }
#else
            glBegin(GL_LINE_STRIP);
            glColor3f(.0f, .0f, .0f);
            foreach(Point_2 p, poly.container())
                glVertex3d(p.x(), p.y(), 0);

            Point_2 first = poly.container().front();
            glVertex3d(first.x(), first.y(), 0);
#endif

            glEnd();
        }
        glPopName();
        if (withTexture) 
			glDisable(GL_TEXTURE_1D);
    }
};


Layer::Layer(System *s, View v, const std::string &a,  bool D3Ojbect, bool asMesh, bool asLine)
    : system(s), view(v),
      attribute(a), vmd(a),
      texture(-1),
      attribute_vector_name(0),
      scale_height(-1),
      as3DObject(D3Ojbect),
      asMesh(asMesh){

    QString attr = QString::fromStdString(a);
    QStringList view_attr = attr.split(":");


    if (view.getType() == DM::COMPONENT || this->as3DObject == true)
        this->rtype = GEOMETRYDRAWER;
    else if (view.getType() == DM::FACE)
	{
		if(this->asMesh)
			this->rtype = MESHDRAWER;
		else if(!this->as3DObject && !this->asMesh)
		{
			if (!asLine)
				this->rtype = DM::TESSELATEDFACEDRAWER;
			else
				this->rtype = DM::FACELINEDRAWER;
		}
    }
    else if (view.getType() == DM::EDGE)
        this->rtype = SIMPLEDRAWEREDGES;
    else if (view.getType() == DM::NODE)
        this->rtype = SIMPLEDRAWERNODES;
    else if (view.getType() == DM::RASTERDATA)
	{
        this->rtype = RASTERDRAWER;
        vmd = ViewMetaData("");
    }

    if (view_attr.size() == 2)
	{
		this->attributeView = *(system->getViewDefinition(view_attr[0].toStdString()));
		this->attribute = view_attr[1].toStdString();

		vmd = ViewMetaData(this->attribute);
	}
}

struct FaceLineDrawer 
{
    GLuint name_start;
    DM::Node * first;
    DM::Node * last;
    FaceLineDrawer(const Layer &l) : name_start(l.getNameStart()) {
    }

    void operator()(DM::System *s, const DM::View& v, DM::Component *cmp, DM::Vector3* point, DM::Vector3* color,  iterator_pos pos) {
		if(pos==in_between)
		{
			if(color)	glColor3dv(&color->x);
			else		glColor3f(0,0,0);
			glVertex3dv(&point->x);
		}
		else if (pos == before) 
		{
            glPushName(name_start);
            glBegin(GL_LINE_STRIP);
            first = 0;
        }
        else if (pos == after) 
		{
            if (first) 
			{
                const double tmp[3] = {first->getX(), first->getY(), first->getZ()};
                glVertex3dv(tmp);
            }
            glEnd();
            glPopName();
            name_start++;
        }
    }
};

struct GeomtryDrawer 
{
    GLuint name_start;

    GeomtryDrawer(const Layer &l) : name_start(l.getNameStart()) {
    }

    void operator()(DM::System *s, const DM::View& v, DM::Component *cmp, DM::Vector3* point, DM::Vector3* color,  iterator_pos pos) {
		if(pos == in_between)
		{
			if(color)	glColor3dv(&color->x);
			else		glColor3f(0,0,0);
			glVertex3dv(&point->x);
		}
		else if (pos == before) 
		{
            glPushName(name_start);
            //glBegin(GL_LINE_STRIP);
            glBegin(GL_TRIANGLES);
        }
        else if (pos == after) 
		{
            glEnd();
            glPopName();
            name_start++;
        }
    }
};

struct RasterDrawer {

    GLuint name_start;
    double current_tex;
    const Layer &l;
    double attr_span;
    RasterDrawer(const Layer &l) : l(l), name_start(l.getNameStart()) 
	{
        const ViewMetaData &vmd = l.getViewMetaData();
        this->attr_span = vmd.attr_max - vmd.attr_min;
    }

    void operator()(DM::System *s, const DM::View& v, DM::Component *cmp, DM::Vector3* point, DM::Vector3* color,  iterator_pos pos) 
	{
		if(pos == in_between)
		{
			if (attr_span != 0) 
			{
				const ViewMetaData &vmd = l.getViewMetaData();
				current_tex = (point->z - vmd.attr_min) / attr_span * 255;
			} 
			else
				current_tex = 0.0;

			if (current_tex < 0) 
			{
				glColor3f(0,0,0);
				glVertex3dv(&point->x);
				return;
			}
			else
			{
				float r = l.LayerColor[(int)current_tex][0]/255.;
				float g = l.LayerColor[(int)current_tex][1]/255.;
				float b = l.LayerColor[(int)current_tex][2]/255.;
				//float a = l.LayerColor[(int)current_tex][3]/255.;
				glColor3f(r, g, b);
				glVertex3dv(&point->x);
			}
		}
        else if (pos == before) 
		{
            const ViewMetaData &vmd = l.getViewMetaData();
            this->attr_span = vmd.attr_max - vmd.attr_min;
            glPushName(name_start);
            glBegin(GL_TRIANGLES);
        }
        else if (pos == after) 
		{
            glEnd();
            glPopName();
            name_start++;
        }
    }
};

struct MeshDrawer 
{
    GLuint name_start;
    double current_tex;
    const Layer &l;
    double attr_span;

    MeshDrawer(const Layer &l)
        : l(l), name_start(l.getNameStart()) 
	{
        if (l.getAttribute() == "")
            return;

        const ViewMetaData &vmd = l.getViewMetaData();
        this->attr_span = vmd.attr_max - vmd.attr_min;
    }

    void operator()(DM::System *s, const DM::View& v, DM::Component *cmp, DM::Vector3* point, DM::Vector3* color,  iterator_pos pos)
	{
		if(pos == in_between)
		{
			if (attr_span != 0) 
			{
				const ViewMetaData &vmd = l.getViewMetaData();
				Attribute *a = cmp->getAttribute(l.getAttribute());

				if (a->getType() == Attribute::DOUBLEVECTOR || a->getType() == Attribute::TIMESERIES)
					current_tex = (a->getDoubleVector()[l.getAttributeVectorName()] - vmd.attr_min) / attr_span *255;
				else
					current_tex = (a->getDouble() - vmd.attr_min) / attr_span * 255;
			}
			else
				current_tex = 0.0;

			if (current_tex < 0)
				glColor3f(0.0, 0.0, 0.0);
			else
			{
				float r = l.LayerColor[(int)current_tex][0]/255.;
				float g = l.LayerColor[(int)current_tex][1]/255.;
				float b = l.LayerColor[(int)current_tex][2]/255.;
				//float a = l.LayerColor[(int)current_tex][3]/255.;
				glColor3f(r, g, b);
			}
			glVertex3dv(&point->x);
		}
        if (pos == before) 
		{
            const ViewMetaData &vmd = l.getViewMetaData();
            this->attr_span = vmd.attr_max - vmd.attr_min;
            glPushName(name_start);
            glBegin(GL_TRIANGLES);
        }
        if (pos == after) 
		{
            glEnd();
            glPopName();
            name_start++;
        }
    }
};


void Layer::draw(QWidget *parent) 
{
    if (lists.size() <= attribute_vector_name)
        lists.resize(attribute_vector_name+1, -1);

    if (!glIsList(lists[attribute_vector_name])) 
	{
        lists[attribute_vector_name] = glGenLists(1);
        glNewList(lists[attribute_vector_name], GL_COMPILE);

		switch(rtype)
		{
		case GEOMETRYDRAWER:{
				GeomtryDrawer drawer(*this);
				iterate_components(system, view, drawer);
				break;
			}
		case MESHDRAWER:{
				MeshDrawer drawer(*this);
				iterate_mesh(system, view, drawer);
				break;
			}
		case FACELINEDRAWER:{
				FaceLineDrawer drawer(*this);
				iterate_faces(system, view, drawer);
				break;
			}
		case TESSELATEDFACEDRAWER:{
				TesselatedFaceDrawer drawer(*this, parent);
				iterate_faces(system, view, drawer);
				break;
			}
		case SIMPLEDRAWERNODES:{
				SimpleDrawer<GL_POINTS> drawer(*this);
				iterate_nodes(system, view, drawer);
				break;
			}
		case SIMPLEDRAWEREDGES:{
				SimpleDrawer<GL_LINES> drawer(*this);
				iterate_edges(system, view, drawer);
				break;
			}
		case RASTERDRAWER:{
				RasterDrawer drawer(*this);
				iterate_rasterdata(system, view, drawer);
				break;
			}
		}
        glEndList();
    }

    glPushMatrix();
    glTranslated(x_off, y_off, z_off);
    assert(glIsList(lists[attribute_vector_name]));
    glCallList(lists[attribute_vector_name]);
    glPopMatrix();
}

void Layer::drawWithNames(QWidget *parent) {
    draw(parent);
}

void Layer::systemChanged() {
    vmd = ViewMetaData(attribute);

	switch(rtype)
	{
	case GEOMETRYDRAWER:
        iterate_components(system, view, vmd);
		break;
	case MESHDRAWER:
        iterate_mesh(system, view, vmd);
		break;
	case TESSELATEDFACEDRAWER:
        iterate_faces(system, view, vmd);
		break;
	case FACELINEDRAWER:
        iterate_faces(system, view, vmd);
		break;
	case SIMPLEDRAWERNODES:
        iterate_nodes(system, view, vmd);
		break;
	case SIMPLEDRAWEREDGES:
        iterate_edges(system, view, vmd);
		break;
	case RASTERDRAWER:
        iterate_rasterdata(system, view, vmd);
		break;
	}
    foreach (GLuint list, lists)
        if (glIsList(list))
            glDeleteLists(list, 1);
}


} // namespace DM
