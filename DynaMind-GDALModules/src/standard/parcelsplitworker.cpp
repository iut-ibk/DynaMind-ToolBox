#include "parcelsplitworker.h"

#include <dmgdalsystem.h>

#include <ogrsf_frmts.h>

#include <iostream>

#include <CGAL/Boolean_set_operations_2.h>
#include <SFCGAL/algorithm/intersection.h>

#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/intersections.h>

ParcelSplitWorker::ParcelSplitWorker()
{
}

ParcelSplitWorker::ParcelSplitWorker(int id, GDALParcelSplit * module, double width,  double target_length, bool splitFirst, char *poly_wkt) :
	id(id),
	module(module),
	width(width),
	splitFirst(splitFirst),
	target_length(target_length),
	poly_wkt(poly_wkt)
{

}


void ParcelSplitWorker::splitePoly(Polygon_with_holes_2 &p)
{
	//Convex Hull (GDAL algorithm doesn't work without)
	Polygon_2 p_c;
	CGAL::convex_hull_2(p.outer_boundary().vertices_begin(), p.outer_boundary().vertices_end(), std::back_inserter(p_c));

    if (p.is_unbounded()) {
        DM::Logger(DM::Error) << "Polygon is unbounded";
        return;
    }

    if (!p_c.is_simple()) {
        DM::Logger(DM::Error) << "Non simple polygon not split";
        return;
    }
    if (!p.outer_boundary().is_simple()) {
        DM::Logger(DM::Error) << "Polygon outerboundary not simple no split";
        return;
    }

	//Cacluate Minimal Rect
	Polygon_2 p_m;
	CGAL::min_rectangle_2(p_c.vertices_begin(), p_c.vertices_end(), std::back_inserter(p_m));

	Pwh_list_2 splitters(this->splitter(p_m));

	foreach (Polygon_with_holes_2 pwh, splitters) {

		Pwh_list_2 split_ress;
        if (pwh.is_unbounded()) {
            DM::Logger(DM::Error) << "Splitted polygon is unbounded no split";
            continue;
        }
        if (!pwh.outer_boundary().is_simple()){
            DM::Logger(DM::Error) << "Polygon outerboundary of splitted polygon not simple no split";
            continue;
        }
		CGAL::intersection(p, pwh,  std::back_inserter(split_ress));
		foreach(Polygon_with_holes_2 pwh_split, split_ress) {
			SFCGAL::Polygon split_geo(pwh_split);
			QString wkt = QString(split_geo.asText(16).c_str());
			module->addToSystem(wkt);
			//emit resultPolygon(wkt);
		}
	}
}


Pwh_list_2 ParcelSplitWorker::splitter(Polygon_2 &rect)
{
	Pwh_list_2 ress;

	Point_2 p1 = rect.vertex(0);
	Point_2 p2 = rect.vertex(1);

	Point_2 p3 = rect.vertex(2);
	Point_2 p4 = rect.vertex(3);

	Vector_2 v1 = (p2-p1);
	Vector_2 v2 = (p3-p2);

	bool v1_bigger = true;
	if (v1.squared_length() < v2.squared_length()) {
		v1_bigger = false;
	}

	int numberOfElements_x  = -1;
	int numberOfElements_y =  -1;

	//Splitt Polygon on the short side in half, or if split_first is false not, and the rest in small peaces of 15m
	if (this->target_length > 0 ){
		numberOfElements_x  = (!v1_bigger) ? sqrt(CGAL::to_double(v1.squared_length())) / this->target_length : sqrt(CGAL::to_double(v1.squared_length()))/this->width;
		numberOfElements_y =  (!v1_bigger) ? sqrt(CGAL::to_double(v2.squared_length())) / this->width : sqrt(CGAL::to_double(v2.squared_length())) / this->target_length;

	} else {
		double splite_width = 2;
		if (!splitFirst)
				splite_width = 1;
		numberOfElements_x  = (!v1_bigger) ? splite_width : sqrt(CGAL::to_double(v1.squared_length())) / this->width;
		numberOfElements_y =  (!v1_bigger) ? sqrt(CGAL::to_double(v2.squared_length())) / this->width : splite_width;
	}
	numberOfElements_x = (numberOfElements_x < 0) ? 1 : numberOfElements_x;
	numberOfElements_y = (numberOfElements_y < 0) ? 1 : numberOfElements_y;

	Vector_2 dv1 = v1/numberOfElements_x;
	Vector_2 dv2 = v2/numberOfElements_y;

	for (int dx = 0; dx < numberOfElements_x; dx++) {
		Vector_2 offsetx = dv1*dx;
		for (int dy = 0; dy < numberOfElements_y; dy++) {
			Polygon_with_holes_2 split_left_1;
			Vector_2 offsety = dv2*dy;
			split_left_1.outer_boundary().push_back(p1+ offsetx + offsety);
			split_left_1.outer_boundary().push_back( p1+dv1 + offsetx + offsety);
			split_left_1.outer_boundary().push_back(p1+dv1+dv2 + offsetx + offsety);
			split_left_1.outer_boundary().push_back(p1+dv2 + offsetx + offsety);
			ress.push_back(split_left_1);
		}
	}
	return ress;
}

void ParcelSplitWorker::run()
{
	//DM::Logger(DM::Standard) << "start " << id;
	std::auto_ptr<  SFCGAL::Geometry > g( SFCGAL::io::readWkt(poly_wkt));
	OGRFree(poly_wkt); //Not needed after here
	switch ( g->geometryTypeId() ) {
	case SFCGAL::TYPE_POLYGON:
		break;
	default:
		return;
	}
	SFCGAL::Polygon poly = g->as<SFCGAL::Polygon>();

	//Transfer to GDAL polygon
	Polygon_with_holes_2 p = poly.toPolygon_with_holes_2(true);

	this->splitePoly(p);
	//DM::Logger(DM::Standard) << "end " << id;
}

