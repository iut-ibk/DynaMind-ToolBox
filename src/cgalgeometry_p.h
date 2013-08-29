/**
 * @file
 * @author  Chrisitan Urich <christian.urich@gmail.com>
 * @version 1.0
 * @section LICENSE
 *
 * This file is part of DynaMind
 *
 * Copyright (C) 2011-2012   Christian Urich
 
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

#ifndef __DynaMind_ToolBox__cgalgeometry_p__
#define __DynaMind_ToolBox__cgalgeometry_p__

#include "dmcompilersettings.h"
#include "dm.h"

#include <list>
#include <fstream>

#include <CGAL/Lazy_exact_nt.h>
#include <CGAL/basic.h>
#include <CGAL/Cartesian.h>
#include <CGAL/Quotient.h>
#include <CGAL/MP_Float.h>
#include <CGAL/Snap_rounding_traits_2.h>
#include <CGAL/Snap_rounding_2.h>
#include <CGAL/Timer.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Arr_segment_traits_2.h>
#include <CGAL/Arr_Bezier_curve_traits_2.h>
#include <CGAL/Arrangement_2.h>
#include <CGAL/IO/Arr_iostream.h>
//#include <CGAL/CORE_algebraic_number_traits.h>
//#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/partition_2.h>
#include <CGAL/Partition_traits_2.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/convex_hull_2.h>
#include <CGAL/bounding_box.h>
#include <CGAL/Aff_transformation_2.h>
#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/Polygon_set_2.h>
#include <CGAL/create_offset_polygons_2.h>

//#include "alglib/src/spline1d.h"


using namespace boost;

//typedef CGAL::Quotient<double>                          Number_type;
//typedef CGAL::Exact_predicates_exact_constructions_kernel<Number_type> k
//Cartesian<Number_type>                    Kernel;

typedef CGAL::Quotient<CGAL::MP_Float>           Number_type;
typedef CGAL::Lazy_exact_nt<CGAL::Quotient<CGAL::MP_Float> > NT;
typedef CGAL::Cartesian<NT>             Kernel;

typedef CGAL::Snap_rounding_traits_2<Kernel>            Traits;
typedef Kernel::Segment_2                               Segment_2;
typedef Kernel::Point_2                                 Point_2;
typedef std::list<Segment_2>                            Segment_list_2;
typedef std::list<Point_2>                              Polyline_2;
typedef std::list<Polyline_2>                           Polyline_list_2;
typedef CGAL::Arr_segment_traits_2<Kernel>              Traits_2;
typedef CGAL::Arrangement_2<Traits_2>                   Arrangement_2;


namespace DM {
class DM_HELPER_DLL_EXPORT CGALGeometry_P
{
public:

	static bool CheckPoints(const Point_2 & p1, const Point_2 & p2);

	static Segment_list_2 Snap_Rounding_2D(DM::System * sys, DM::View &view, float tol);

	static int CountNeighboringVertices (Arrangement_2::Vertex_const_handle v);

	static Segment_list_2 EdgeToSegment2D(DM::System * sys,  DM::View &view);

	static DM::System Segment2DToEdge(Segment_list_2 seg_list, DM::View &view);

	static Segment_list_2 PolyLineToSegments(const Polyline_list_2 & poly_list);

	static double NumberTypetoFloat(Number_type n);

	static void AddFaceToArrangement(Arrangement_2 & arr, DM::Face * f);






	/*static VectorData  DrawTemperaturAnomaly(Point p, double l1, double l2, double b, double T);
	 static VectorData createRaster(std::vector<Point> & points, double width, double height);*/




};
}

#endif /* defined(__DynaMind_ToolBox__cgalgeometry_p__) */
