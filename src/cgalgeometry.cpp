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
#include "cgalgeometry.h"
#include <sstream>
#include <ostream>
#include <math.h>

#include <QString>
#include <boost/foreach.hpp>
#include <iostream>
#include "cgalgeometry_p.h"
#include "tbvectordata.h"
#include "cgaltriangulation.h"
#include "cgalregulartriangulation.h"
#include <CGAL/min_quadrilateral_2.h>
#include <CGAL/intersections.h>
#include <CGAL/Polygon_set_2.h>
#include <print_utils.h>
#include <CGAL/Polygon_2_algorithms.h>
#include <CGAL/Projection_traits_xy_3.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Point_2.h>
#include <CGAL/Aff_transformation_2.h>
#include <CGAL/Polygon_with_holes_2.h>
#include <CGAL/centroid.h>







namespace DM {


DM::System CGALGeometry::ShapeFinder(DM::System * sys, DM::View & id, DM::View & return_id, bool withSnap_Rounding,  float Tolerance, bool RemoveLines)  {

    Arrangement_2::Edge_iterator					eit;
    Arrangement_2::Face_const_iterator              fit;
    Arrangement_2::Ccb_halfedge_const_circulator    curr;
    Segment_list_2 segments;
    Arrangement_2                                   arr;


    DM::System return_vec;

    if (withSnap_Rounding == true) {
        segments = CGALGeometry_P::Snap_Rounding_2D(sys, id, Tolerance);
    } else {
        segments = CGALGeometry_P::EdgeToSegment2D(sys, id);
    }
    insert (arr, segments.begin(), segments.end());
    if (RemoveLines == true){

        int removecounter = 0;
        do {
            removecounter = 0;
			std::list<Arrangement_2::Edge_iterator> removeList;

            for (eit = arr.edges_begin(); eit != arr.edges_end(); ++eit) {
                Arrangement_2::Vertex_handle   v1 = eit->source(), v2 = eit->target();
                int c1 = CGALGeometry_P::CountNeighboringVertices(v1);
                int c2 = CGALGeometry_P::CountNeighboringVertices(v2);
                if (c1 < 2 || c2 < 2) {
                    //arr.remove_edge (eit);
					removeList.push_back(eit);
                    removecounter++;
                }
            }
			foreach(Arrangement_2::Edge_iterator it, removeList)
				arr.remove_edge(it);

            DM::Logger(DM::Debug)<< "Removed Edges with lose end " << removecounter;

        } while (removecounter > 0);
    }
    int faceCounter = 0;
    for (fit = arr.faces_begin(); fit != arr.faces_end(); ++fit) {
        if (fit->is_unbounded()) {
            continue;
        }
        curr = fit->outer_ccb();

        Arrangement_2::Ccb_halfedge_const_circulator hec = fit->outer_ccb();
        Arrangement_2::Ccb_halfedge_const_circulator end = hec;
        Arrangement_2::Ccb_halfedge_const_circulator next = hec;
        std::vector<Point_2> ressults_P2;
        std::vector<DM::Node *> vp;

        next++;
        if (hec->curve().target() == next->curve().source() || hec->curve().target() == next->curve().target()) {
            ressults_P2.push_back(hec->curve().target());
            float x = CGAL::to_double(hec->curve().target().x());
            float y = CGAL::to_double(hec->curve().target().y());
            vp.push_back(return_vec.addNode(x,y,0));

        } else {
            ressults_P2.push_back(hec->curve().source());
            float x = CGAL::to_double(hec->curve().source().x());
            float y = CGAL::to_double(hec->curve().source().y());
            vp.push_back(return_vec.addNode(x,y,0));
        }
        do{

            ++hec;
            bool source = false;
            bool target = false;
            for ( unsigned int i = 0; i < ressults_P2.size(); i++) {
                if ( ressults_P2[i] == hec->curve().target() ) {
                    target = true;
                }  else if ( ressults_P2[i] == hec->curve().source() )   {
                    source = true;
                }

            }

            if (source == false ) {
                ressults_P2.push_back(hec->curve().source());
                float x = CGAL::to_double(hec->curve().source().x());
                float y = CGAL::to_double(hec->curve().source().y());
                vp.push_back(return_vec.addNode(x,y,0));
            }
            if (target == false ) {
                ressults_P2.push_back(hec->curve().target());
                float x = CGAL::to_double(hec->curve().target().x());
                float y = CGAL::to_double(hec->curve().target().y());
                vp.push_back(return_vec.addNode(x,y,0));
            }

        }
        while(hec != end );
        faceCounter++;
        if (vp.size() < 3)
            continue;
        vp.push_back(vp[0]);
        return_vec.addFace(vp, return_id);

    }
    DM::Logger(DM::Debug)<< "Number of extracted Faces " << faceCounter;
    return return_vec;
}

double CGALGeometry::CalculateMinBoundingBox(std::vector<Node*> nodes, std::vector<DM::Node> & boundingBox, std::vector<double> & size) {

    typedef CGAL::Exact_predicates_inexact_constructions_kernel K ;
    typedef K::Point_2                                          Point_2;
    typedef CGAL::Polygon_2<K>                                  Polygon_2;

    const double pi =  3.14159265358979323846;
    double angel = 0;
    double l  = 0;
    double w = 0;

    Polygon_2 pls;
    std::vector<Point_2> lpoints;
    unsigned int s_nodes = nodes.size();
	if (nodes[0] == nodes[s_nodes])
        s_nodes--;
    for (unsigned int i = 0; i < s_nodes; i++) {
        DM::Node * n = nodes[i];
        lpoints.push_back(Point_2(n->getX(), n->getY()));
    }

    CGAL::convex_hull_2( lpoints.begin(), lpoints.end(), std::back_inserter(pls) );
    if (!pls.is_simple()) {
        return -1;
    }
    Polygon_2 p_m;
    CGAL::min_rectangle_2(
                pls.vertices_begin(), pls.vertices_end(), std::back_inserter(p_m));
    for (Polygon_2::Vertex_const_iterator vit = p_m.vertices_begin(); vit!= p_m.vertices_end(); vit++) {
        boundingBox.push_back(DM::Node(CGAL::to_double(vit->x()), CGAL::to_double(vit->y()), 0));
    }

    l = TBVectorData::calculateDistance(&boundingBox[0], &boundingBox[1]);
    w = TBVectorData::calculateDistance(&boundingBox[0], &boundingBox[3]);


    angel = TBVectorData::AngelBetweenVectors(DM::Node(1,0,0), boundingBox[1]-boundingBox[0])*180./pi;

    //DM::Logger(DM::Debug) << l << " " << w << " " << angel;
    if (l < w) {
        angel += 90;
        double tmp_l = l;
        l = w;
        w = tmp_l;

    }

    size.push_back(l);
    size.push_back(w);
    return angel;
}

std::vector<Node> CGALGeometry::OffsetPolygon(std::vector<Node*> points, double offset)  {
    typedef CGAL::Exact_predicates_inexact_constructions_kernel K ;
    typedef K::Point_2                                          Point_2;
    typedef CGAL::Aff_transformation_2<K>                       Transformation;
    typedef CGAL::Polygon_2<K>                                  Polygon_2;
    typedef CGAL::Polygon_set_2<K>                              Polygon_set_2;
    typedef CGAL::Polygon_with_holes_2<K>                       Polygon_with_holes_2;
    typedef std::list<Polygon_with_holes_2>                     Pwh_list_2;
    typedef CGAL::Straight_skeleton_2<K>                        Ss ;
    typedef boost::shared_ptr<Polygon_2>                        PolygonPtr ;
    typedef boost::shared_ptr<Ss>                               SsPtr ;
    typedef std::vector<PolygonPtr>                             PolygonPtrVector ;

    Polygon_2 poly_s;
    std::vector<Node> ret_points;
    unsigned int vector_size = points.size();
	if (points[0] == points[vector_size] )
        vector_size--;
    if (offset == 0) {
        for (unsigned int i = 0; i < vector_size; i++ ) {
            ret_points.push_back(Node(points[i]->getX(), points[i]->getY(), 0));
        }
        return ret_points;
    }
	double v[3];
    for (unsigned int i = 0; i <  vector_size; i++) {
		points[i]->get(v);
        poly_s.push_back(Point_2(v[0], v[1]));
    }

    if(!poly_s.is_simple()) {
        Logger(Warning) << "Can't perform offset polygon is not simple";
        return ret_points;
    }
    CGAL::Orientation orient = poly_s.orientation();
    if (orient == CGAL::CLOCKWISE) {
        poly_s.reverse_orientation();
    }
    SsPtr ss = CGAL::create_interior_straight_skeleton_2(poly_s);

    PolygonPtrVector offset_polygons = CGAL::create_offset_polygons_2<Polygon_2>(offset,*ss);

    foreach(PolygonPtr poly, offset_polygons) {
        Polygon_2 p = *(poly);
        for (unsigned int i = 0; i < p.size(); i++ ) {
            ret_points.push_back(Node(p[i].x(), p[i].y(), 0));
        }
    }
    return ret_points;
}

std::vector<DM::Node> CGALGeometry::FaceTriangulation(System *sys, Face *f)
{
    std::vector<DM::Node> triangles;

    CGALTriangulation::Triangulation(sys, f, triangles);
    return triangles;
}

void CGALGeometry::FaceTriangulation(System *sys, Face *f, std::vector<DM::Node> &triangles)
{
    CGALTriangulation::Triangulation(sys, f, triangles);
}

std::vector<DM::Node> CGALGeometry::RegularFaceTriangulation(System *sys, Face *f, std::vector<int> & ids, double meshsize)
{
    std::vector<DM::Node> triangles;

    CGALRegularTriangulation::Triangulation(sys, f, triangles,  meshsize, ids);
    return triangles;
}

bool CGALGeometry::DoFacesInterect(std::vector<DM::Node*> nodes1, std::vector<DM::Node*> nodes2) {
    typedef CGAL::Exact_predicates_exact_constructions_kernel K;
    typedef K::Point_2                                          Point;
    typedef CGAL::Polygon_2<K>                                  Polygon_2;
    typedef CGAL::Polygon_set_2<K, std::vector<Point> >         Polygon_set_2;
    typedef CGAL::Polygon_with_holes_2<K>                       Polygon_with_holes_2;
    typedef std::list<Polygon_with_holes_2>                     Pwh_list_2;

    int size_n1 = nodes1.size();
    Polygon_2 poly1;

	for (int i = 0; i < size_n1; i++) {
        DM::Node * n = nodes1[i];
        poly1.push_back(Point(n->getX(), n->getY()));
    }

    int size_n2 = nodes2.size();

    Polygon_2 poly2;
	for (int i = 0; i < size_n2; i++) {
        DM::Node * n = nodes2[i];
        poly2.push_back(Point(n->getX(), n->getY()));
    }

    if (!poly1.is_simple()) {
        Logger(Debug) << "Polygon1 is not simple cant perform intersection";
        return true;
    }

    if (!poly2.is_simple()) {
        Logger(Debug) << "Polygon2 is not simple cant perform intersection";
        return true;
    }

    CGAL::Orientation orient = poly1.orientation();
    if (orient == CGAL::CLOCKWISE) {
        poly1.reverse_orientation();
    }

    orient = poly2.orientation();
    if (orient == CGAL::CLOCKWISE) {
        poly2.reverse_orientation();
    }

    return CGAL::do_intersect (poly1, poly2);
}

std::vector<DM::Face*> CGALGeometry::IntersectFace(System *sys, Face *f1, Face *f2)
{

   std::vector<DM::Face *> resultFaces;

    typedef CGAL::Exact_predicates_exact_constructions_kernel K;
    typedef K::Point_2                                          Point;
    typedef CGAL::Polygon_2<K>                                  Polygon_2;
    typedef CGAL::Polygon_set_2<K, std::vector<Point> >         Polygon_set_2;
    typedef CGAL::Polygon_with_holes_2<K>                       Polygon_with_holes_2;
    typedef std::list<Polygon_with_holes_2>                     Pwh_list_2;

    Polygon_2::Vertex_iterator vit;
    Polygon_with_holes_2::Hole_iterator hit;


	std::vector<DM::Node*> nodes1 = TBVectorData::getNodeListFromFace(sys, f1);

	int size_n1 = nodes1.size();

    Polygon_2 poly1;

	for (int i = 0; i < size_n1; i++) {
        DM::Node * n = nodes1[i];
        poly1.push_back(Point(n->getX(), n->getY()));
    }

    std::vector<DM::Node*> nodes2 = TBVectorData::getNodeListFromFace(sys, f2);

    int size_n2 = nodes2.size();

    Polygon_2 poly2;

	for (int i = 0; i < size_n2; i++) {
        DM::Node * n = nodes2[i];
        poly2.push_back(Point(n->getX(), n->getY()));
    }


    if (!poly1.is_simple()) {
        Logger(Debug) << "Polygon1 is not simple cant perform intersection";
        return resultFaces;
    }
    if (!poly2.is_simple()) {
        Logger(Debug) << "Polygon2 is not simple cant perform intersection";

        return resultFaces;
    }

    CGAL::Orientation orient = poly1.orientation();
    if (orient == CGAL::CLOCKWISE) {
        poly1.reverse_orientation();
    }

    orient = poly2.orientation();
    if (orient == CGAL::CLOCKWISE) {
        poly2.reverse_orientation();
    }


    Polygon_with_holes_2 p_holes1(poly1);

    foreach (DM::Face * h, f1->getHolePointers()) {
        Polygon_2 poly_h;
        std::vector<DM::Node*> nodes_h = TBVectorData::getNodeListFromFace(sys, h);
        int s = nodes_h.size();
		for (int i = 0; i < s; i++) {
            DM::Node * n = nodes_h[i];
            poly_h.push_back(Point(n->getX(), n->getY()));
        }

        orient = poly_h.orientation();
        if (orient == CGAL::COUNTERCLOCKWISE) {
            poly_h.reverse_orientation();
        }

        p_holes1.add_hole(poly_h);
    }

    Polygon_with_holes_2 p_holes2(poly2);
    foreach (DM::Face * h, f2->getHolePointers()) {
        Polygon_2 poly_h;
        std::vector<DM::Node*> nodes_h = TBVectorData::getNodeListFromFace(sys, h);
        int s = nodes_h.size();
		for (int i = 0; i < s; i++) {
            DM::Node * n = nodes_h[i];
            poly_h.push_back(Point(n->getX(), n->getY()));
        }

        orient = poly_h.orientation();
        if (orient == CGAL::COUNTERCLOCKWISE) {
            poly_h.reverse_orientation();
        }

        p_holes2.add_hole(poly_h);
    }

    Pwh_list_2                  intR;
    Pwh_list_2::const_iterator  it;


    CGAL::intersection (p_holes1, p_holes2, std::back_inserter(intR));

    int counter = 0;


    for (it = intR.begin(); it != intR.end(); ++it) {

        Polygon_with_holes_2 P = (*it);
        std::vector<DM::Node *> currentNodes;
        Polygon_2 P_out = P.outer_boundary();
        for (vit = P_out.vertices_begin(); vit !=P_out.vertices_end(); ++vit) {
            DM::Node tmp(CGAL::to_double(vit->x()), CGAL::to_double(vit->y()), 0);
            bool exists = false;
            foreach (DM::Node * n, currentNodes) {
                if (n->compare2d(tmp,0.00001))
                    exists = true;
            }
            if (!exists)
                currentNodes.push_back(sys->addNode(CGAL::to_double(vit->x()), CGAL::to_double(vit->y()), 0));
        }
        int n_holes = P.number_of_holes();
        Logger(DM::Debug) << "Holes" << n_holes;


		if (currentNodes.size() < 2) {
            DM::Logger(DM::Error) << "Something went wrong";
            continue;
        }
        counter++;
        currentNodes.push_back(currentNodes[0]);
        DM::Face * f = sys->addFace(currentNodes);

        //Add Holes
        for (hit = P.holes_begin(); hit != P.holes_end(); ++hit)
        {
            std::vector<DM::Node *> currentNodes_holes;
            Polygon_2 hole = *hit;
            for (vit = hole.vertices_begin(); vit !=hole.vertices_end(); ++vit) {
                DM::Node tmp(CGAL::to_double(vit->x()), CGAL::to_double(vit->y()), 0);
                bool exists = false;
                foreach (DM::Node * n, currentNodes_holes) {
                    if (n->compare2d(tmp,0.00001))
                        exists = true;
                }
                if (!exists)
                    currentNodes_holes.push_back(sys->addNode(CGAL::to_double(vit->x()), CGAL::to_double(vit->y()), 0));
            }
			if (currentNodes_holes.size() < 2) {
                DM::Logger(DM::Error) << "Something went wrong with a hole";
                continue;
            }
            currentNodes_holes.push_back(currentNodes_holes[0]);
            f->addHole(currentNodes_holes);
        }
        resultFaces.push_back(f);

    }
    DM::Logger(DM::Debug) << "NumberOfFaces " << counter;

    return resultFaces;
}

std::vector<DM::Node> CGALGeometry::RotateNodes(std::vector<DM::Node>  nodes, double alpha)
{

    typedef CGAL::Cartesian<double>         K;
    typedef CGAL::Aff_transformation_2<K>   Transformation;
    typedef K::Point_2                  Point;

    std::vector<DM::Node> ressVec;
    const double pi =  3.14159265358979323846;

    Transformation rotate(CGAL::ROTATION,  sin(alpha/180*pi), cos(alpha/180*pi));

    foreach (DM::Node n, nodes) {

        Point q(n.getX(), n.getY());
        q = rotate(q);

        ressVec.push_back(DM::Node(q.x(), q.y(), n.getZ()));


    }
    return ressVec;



}

bool CGALGeometry::CheckOrientation(std::vector<DM::Node*> nodes)
{
    typedef CGAL::Exact_predicates_exact_constructions_kernel K;
    typedef K::Point_2                                          Point;
    typedef CGAL::Polygon_2<K>                                  Polygon_2;
    typedef CGAL::Polygon_set_2<K, std::vector<Point> >         Polygon_set_2;
    typedef CGAL::Polygon_with_holes_2<K>                       Polygon_with_holes_2;
    typedef std::list<Polygon_with_holes_2>                     Pwh_list_2;


    int size_n1 = nodes.size();

    Polygon_2 poly1;
	double v[3];
	for (int i = 0; i < size_n1; i++) {
        //DM::Node * n = nodes[i];
        //poly1.push_back(Point(n->getX(), n->getY()));
		nodes[i]->get(v);
        poly1.push_back(Point(v[0], v[1]));
    }


    CGAL::Orientation orient = poly1.orientation();
    if (orient == CGAL::CLOCKWISE) {
        return false;
    }

    return true;

}

Node CGALGeometry::CalculateCentroid(System *sys, Face *f)
{
	typedef CGAL::Exact_predicates_inexact_constructions_kernel   K;
	typedef K::Point_3                                          Point_3;

	std::vector<DM::Node *> nodes = f->getNodePointers();

	if (nodes.size() == 0) {
		Logger(Warning) << "No nodes given";
		return Node(0,0,0);
	}

	std::list<Point_3>  poly1;
	double v[3];

	for (int i = 0; i < nodes.size(); i++) {
		nodes[i]->get(v);
		poly1.push_back(Point_3(v[0], v[1], v[2]));
	}
	Point_3 c2 = CGAL::centroid(poly1.begin(), poly1.end(),CGAL::Dimension_tag<0>());
	foreach (Point_3 p , poly1) {
		DM::Logger(DM::Debug) << c2.x()<< " " << c2.y() << " " << c2.z();
	}

	DM::Logger(DM::Debug) << c2.x()<< " " << c2.y() << " " << c2.z();
	return DM::Node(c2.x(), c2.y(), c2.z());
}


void CGALGeometry::CalculateCentroid(System *sys, Face *f, double &x, double &y, double &z)
{
    typedef CGAL::Exact_predicates_inexact_constructions_kernel   K;
	typedef K::Point_3                                          Point_3;


	std::vector<DM::Node *> nodes = f->getNodePointers();

	if (nodes.size() == 0) {
		Logger(Warning) << "No nodes given";
		return;
	}

	std::list<Point_3>  poly1;

	double v[3];
	for (int i = 0; i < nodes.size(); i++) {
		nodes[i]->get(v);
		poly1.push_back(Point_3(v[0], v[1], v[2]));
	}
	Point_3 c2 = CGAL::centroid(poly1.begin(), poly1.end(),CGAL::Dimension_tag<0>());

	x = c2.x();
	y = c2.y();
	z = c2.z();
}

double CGALGeometry::CalculateArea2D(Face *f)
{
	typedef CGAL::Exact_predicates_inexact_constructions_kernel   K;
	typedef K::Point_2                                        Point;

	std::vector<Point> poly1;

	foreach (DM::Node * n, f->getNodePointers()) {
		poly1.push_back(Point(n->getX(), n->getY()));
	}
	double area = 0;
	double tmp_area = 0;
	CGAL::area_2(poly1.begin(), poly1.end(),tmp_area);
	area+=fabs(tmp_area);
	foreach (DM::Face * h, f->getHolePointers()) {
		std::vector<Point> poly_h;
		foreach (DM::Node * n, h->getNodePointers()) {
			poly_h.push_back(Point(n->getX(), n->getY()));
		}
		CGAL::area_2(poly_h.begin(), poly_h.end(),tmp_area);
		area-=fabs(tmp_area);
	}
	return area;
}

}
