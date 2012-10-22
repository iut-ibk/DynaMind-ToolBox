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
#include <QString>
#include <boost/foreach.hpp>
#include <iostream>
#include "cgalgeometry_p.h"

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
        segments = CGALGeometry_P::VecToSegment2D(sys, id);
    }
    insert (arr, segments.begin(), segments.end());
    if (RemoveLines == true){

        int removecounter = 0;
        do {
            removecounter = 0;
            for (eit = arr.edges_begin(); eit != arr.edges_end(); ++eit) {
                Arrangement_2::Vertex_handle   v1 = eit->source(), v2 = eit->target();
                int c1 = CGALGeometry_P::CountNeighboringVertices(v1);
                int c2 = CGALGeometry_P::CountNeighboringVertices(v2);
                if (c1 < 2 || c2 < 2) {
                    arr.remove_edge (eit);
                    removecounter++;
                }
            }
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

double CGALGeometry::CalculateMinBoundingBox(std::vector<Node*> nodes, std::vector<DM::Node> & boundingBox) {
    typedef double                          FT;
    typedef CGAL::Cartesian<FT>             K;
    typedef K::Point_2                      Point_2;
    typedef CGAL::Aff_transformation_2<K>   Transformation;
    const double pi =  3.14159265;
    double area = -1;
    double angel = 0;
    for (double i = 0; i < 180; i++) {
        Transformation rotate(CGAL::ROTATION, sin(i/180.*pi), cos(i/180.*pi));
        std::list<Point_2> points_2;

        foreach(Node * n, nodes) {
            points_2.push_back(rotate(Point_2(n->getX(), n->getY())));
        }

        K::Iso_rectangle_2 c2 = CGAL::bounding_box(points_2.begin(), points_2.end());
        double a = c2.area();
        if (area < 0 || a < area ) {
            area = a;
            angel = i;
        }

    }

    //caluclate final bounding box
    Transformation rotate(CGAL::ROTATION, sin(angel/180.*pi), cos(angel/180.*pi));
    std::list<Point_2> points_2;
    foreach(Node * n, nodes) {
        points_2.push_back(rotate(Point_2(n->getX(), n->getY())));
    }
    K::Iso_rectangle_2 c2 = CGAL::bounding_box(points_2.begin(), points_2.end());
    for (int i = 0; i < 4; i++) {
        Point_2 p = c2.vertex(0);
        boundingBox.push_back(DM::Node(CGAL::to_double(p.x()),CGAL::to_double(p.y()), 0));
    }

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
    foreach(Node * p, points) {
        poly_s.push_back(Point_2(p->getX(), p->getY()));
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
        for (int i = 0; i < p.size(); i++ ) {
            ret_points.push_back(Node(p[i].x(), p[i].y(), 0));
        }
    }
    return ret_points;
}


std::vector<Face*> CGALGeometry::ExtrudeFace(DM::System * sys, const DM::View & view, const std::vector<DM::Node*> &vp, const float & height, bool withLid){
    //Create Upper Points
    std::vector<DM::Node*> opposite_ids;
    //Face refF = vf[0];
    foreach(DM::Node * n, vp) {
        DM::Node * n_new = sys->addNode(n->getX(), n->getY(), n->getZ() + height);
        opposite_ids.push_back(n_new);
    }

    //Create Sides
    std::vector<DM::Face*> newFaces;
    for (int i = 0; i < vp.size(); i++) {
        if (i != 0) {
            std::vector<Node *> f_side;
            f_side.push_back(vp[i]);
            f_side.push_back(opposite_ids[i]);
            f_side.push_back(opposite_ids[i-1]);
            f_side.push_back(vp[i-1]);
            f_side.push_back(vp[i]);
            newFaces.push_back(sys->addFace(f_side, view));
        }

    }

    //Create Lid
    if (withLid)
        newFaces.push_back(sys->addFace(opposite_ids, view));

    return newFaces;


}
}
