/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2011-2012  Christian Urich

 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */
#include "cgalskeletonisation.h"
#include <sstream>
#include <ostream>
#define _USE_MATH_DEFINES
#include <math.h>

#include <QString>
#include <boost/foreach.hpp>
#include <iostream>
#include "tbvectordata.h"

#include <dmgeometry.h>
#include <print_utils.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Point_2.h>
#include <CGAL/create_offset_polygons_2.h>
#include <cgalgeometry.h>

namespace DM {

DM::System CGALSkeletonisation::StraightSkeletonisation(System *sys, Face *f, double alpha)
{
	DM::System sys_tmp;

	typedef CGAL::Exact_predicates_inexact_constructions_kernel K ;

	typedef K::Point_2                   Point_2 ;
	typedef CGAL::Polygon_2<K>           Polygon_2 ;
	typedef CGAL::Straight_skeleton_2<K> Ss ;

	typedef  Ss::Vertex_const_handle     Vertex_const_handle ;
	typedef  Ss::Halfedge_const_handle   Halfedge_const_handle ;
	typedef  Ss::Halfedge_const_iterator Halfedge_const_iterator ;
	typedef  Ss::Face_const_handle        Face_const_handle;
	typedef  Ss::Face_const_iterator      Face_const_iterator;
	typedef  boost::shared_ptr<Ss> SsPtr ;
	typedef K::FT                        FT ;
	typedef boost::shared_ptr<Polygon_2> PolygonPtr ;
	typedef std::vector<PolygonPtr> PolygonPtrVector ;


	DM::SpatialNodeHashMap sphn(&sys_tmp, 10.);

	std::vector<DM::Node> ressVector;
	DM::View view_roof_lines("Roof_Edges", DM::EDGE, DM::WRITE);
	DM::View view_roof_faces("Roof", DM::FACE, DM::WRITE);
	Polygon_2 poly_s;


	foreach(DM::Node * p, f->getNodePointers()) {
		poly_s.push_back(Point_2(p->getX(), p->getY()));
		sphn.addNode(p->getX(), p->getY(), p->getZ(),0.0001, DM::View());
	}

	if(!poly_s.is_simple()) {
		Logger(Warning) << "Can't perform offset polygon is not simple";
		return sys_tmp;
	}

	CGAL::Orientation orient = poly_s.orientation();
	if (orient == CGAL::CLOCKWISE) {
		poly_s.reverse_orientation();
	}

	// You can pass the polygon via an iterator pair
	SsPtr iss = CGAL::create_interior_straight_skeleton_2(poly_s.vertices_begin(), poly_s.vertices_end());
//	std::vector<DM::Node*> new_nodes;
	for ( Halfedge_const_iterator i = iss->halfedges_begin(); i !=  iss->halfedges_end(); ++i )
	{
		Point_2 p = i->opposite()->vertex()->point();

		Logger(Debug) << "(" << p.x() << "," << p.y() << ")" ;
		Logger(Debug) << "->" ;
		Point_2 p1 = i->vertex()->point();
		Logger(Debug) << "(" << p1.x() << "," << p1.y() << ")" ;
		Logger(Debug) << " " << ( i->is_bisector() ? "bisector" : "contour" );
		double dx = p1.x() -  p.x();
		double dy = p1.y() -  p.y();
		double l = sqrt(dx * dx + dy * dy);
		double h = l*cos(30./180.*M_PI);
		ressVector.push_back(DM::Node(p.x(), p.y(),0));

//		if (!sphn.findNode(p.x(), p.y(), 0.0001)) {
//			Logger(Debug) <<   "(" << p.x() << "," << p.y() << ")" ;
//			Logger(Debug) << "Height" << h;
//			new_nodes.push_back(sphn.addNode(p.x(), p.y(), h, 0.0001,  DM::View()));
//		}
//		if (!sphn.findNode(p1.x(), p1.y(), 0.0001)) {
//			Logger(Debug) <<   "(" << p1.x() << "," << p1.y() << ")" ;
//			Logger(Debug) << "Height" << h;
//			new_nodes.push_back(sphn.addNode(p1.x(), p1.y(), h, 0.0001,  DM::View()));
//		}
		DM::Node * n1 = sphn.addNode(p.x(), p.y(), h, 0.0001,  DM::View());
		DM::Node * n2 = sphn.addNode(p1.x(), p1.y(), h, 0.0001,  DM::View());
		sys_tmp.addEdge(n1, n2, view_roof_lines);

	}
	DM::System shapes = DM::CGALGeometry::ShapeFinder(&sys_tmp,view_roof_lines,view_roof_faces, false, 0.0001  );

	Logger(Debug) << "number of faces in roof " << shapes.getUUIDs(view_roof_faces).size();

	std::vector<std::string> uuids =  shapes.getUUIDs(view_roof_faces);


	foreach (std::string uuid, uuids) {
		std::vector<DM::Node*> faces;
		DM::Face * f = shapes.getFace(uuid);
		std::vector<DM::Node*> nodes_in_face = TBVectorData::getNodeListFromFace(&shapes, f);
		foreach (DM::Node * n, nodes_in_face) {
			faces.push_back(sphn.addNode(n->getX(), n->getY(), n->getZ(), 0.0001,  DM::View()));
		}

		sys_tmp.addFace(faces, view_roof_faces);
	}


	return sys_tmp;
}

}
