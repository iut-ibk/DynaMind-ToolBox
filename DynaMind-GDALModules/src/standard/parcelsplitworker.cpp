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

ParcelSplitWorker::ParcelSplitWorker(GDALParcelSplit * module, double width, bool splitFirst, char *poly_wkt) :
	module(module),
	width(width),
	splitFirst(splitFirst),
	poly_wkt(poly_wkt)
{

}


void ParcelSplitWorker::splitePoly(Polygon_with_holes_2 &p)
{
	//Convex Hull (GDAL algorithm doesn't work without)
	Polygon_2 p_c;
	CGAL::convex_hull_2(p.outer_boundary().vertices_begin(), p.outer_boundary().vertices_end(), std::back_inserter(p_c));

	//Cacluate Minimal Rect
	Polygon_2 p_m;
	CGAL::min_rectangle_2(p_c.vertices_begin(), p_c.vertices_end(), std::back_inserter(p_m));

	Pwh_list_2 splitters(this->splitter(p_m));

	foreach (Polygon_with_holes_2 pwh, splitters) {
		Pwh_list_2 split_ress;
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

	//Splitt Polygon on the short side in half, or if split_first is false not, and the rest in small peaces of 15m
	double splite_width = 2;
	if (!splitFirst)
			splite_width = 1;

	int numberOfElements_x  = (!v1_bigger) ? splite_width : sqrt(CGAL::to_double(v1.squared_length()))/this->width;
	int numberOfElements_y =  (!v1_bigger) ? sqrt(CGAL::to_double(v2.squared_length()))/this->width : splite_width;

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
	std::auto_ptr<  SFCGAL::Geometry > g( SFCGAL::io::readWkt(poly_wkt));

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
}

