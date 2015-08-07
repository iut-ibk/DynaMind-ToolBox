#include "parcelsplitter.h"

#include <dmgdalsystem.h>

#include <ogrsf_frmts.h>

#include <iostream>

#include <CGAL/Boolean_set_operations_2.h>
#include <SFCGAL/algorithm/intersection.h>

#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/intersections.h>

ParcelSplitter::ParcelSplitter() {

}

ParcelSplitter::ParcelSplitter(double width, double length, Polygon_with_holes_2 p):
  width(width),
  length(length),
  //results_vector(results_vector),
  poly(p)
{
}


void ParcelSplitter::splitePoly( Polygon_with_holes_2 &p)
{
	//Convex Hull (GDAL algorithm doesn't work without)

	Polygon_2 p_c;
	CGAL::convex_hull_2(p.outer_boundary().vertices_begin(), p.outer_boundary().vertices_end(), std::back_inserter(p_c));

	//Cacluate Minimal Rect
	Polygon_2 p_m;
	CGAL::min_rectangle_2(p_c.vertices_begin(), p_c.vertices_end(), std::back_inserter(p_m));

	Pwh_list_2 splitters(this->splitter(p_m));

	if (splitters.size() == 0) {
		SFCGAL::Polygon split_geo(p);
		//results_vector->push_back(split_geo);
		//DM::Logger(DM::Error) << "Emit";
		QString wkt = QString::fromLatin1(split_geo.asText(9).c_str());
		emit resultPolygon(wkt);
		//addToSystem(split_geo);
		return;
	}

	foreach (Polygon_with_holes_2 pwh, splitters) {
		Pwh_list_2 split_ress;
		CGAL::intersection(p, pwh,  std::back_inserter(split_ress));
		foreach(Polygon_with_holes_2 pwh_split, split_ress) {
			if (!pwh_split.outer_boundary().is_simple()) {
				DM::Logger(DM::Error) << "Non simple polygon not split";
				continue;
			}
			splitePoly(pwh_split);
		}
	}
	return;

}

void ParcelSplitter::run()
{
	this->splitePoly(poly);
}



void ParcelSplitter::split_left(Point_2 & p3, Pwh_list_2 & ress, Point_2 & p2, Point_2 & p4, Point_2 & p1, Vector_2 & v1)
{
	Polygon_with_holes_2 split_left;
	split_left.outer_boundary().push_back(p1);
	split_left.outer_boundary().push_back(p1+v1);
	split_left.outer_boundary().push_back(p4+v1);
	split_left.outer_boundary().push_back(p4);
	ress.push_back(split_left);

	//Right
	Polygon_with_holes_2 split_right;
	split_right.outer_boundary().push_back(p2-v1);
	split_right.outer_boundary().push_back(p2);
	split_right.outer_boundary().push_back(p3);
	split_right.outer_boundary().push_back(p3-v1);
	ress.push_back(split_right);
}

void ParcelSplitter::split_up(Pwh_list_2 & ress, Point_2 & p3, Point_2 & p1, Point_2 & p4, Vector_2 & v2, Point_2 & p2)
{
	Polygon_with_holes_2 split_left_1;
	split_left_1.outer_boundary().push_back(p1);
	split_left_1.outer_boundary().push_back(p2);
	split_left_1.outer_boundary().push_back(p2+v2);
	split_left_1.outer_boundary().push_back(p1+v2);
	ress.push_back(split_left_1);

	//Right
	Polygon_with_holes_2 split_right_1;
	split_right_1.outer_boundary().push_back(p3-v2);
	split_right_1.outer_boundary().push_back(p3);
	split_right_1.outer_boundary().push_back(p4);
	split_right_1.outer_boundary().push_back(p4-v2);
	ress.push_back(split_right_1);
}

Pwh_list_2 ParcelSplitter::splitter(Polygon_2 &rect)
{
	Pwh_list_2 ress;

	Point_2 p1 = rect.vertex(0);
	Point_2 p2 = rect.vertex(1);

	Point_2 p3 = rect.vertex(2);
	Point_2 p4 = rect.vertex(3);

	Vector_2 v1 = (p2-p1)/2;
	Vector_2 v2 = (p3-p2)/2;

	bool v1_bigger = true;
	if (v1.squared_length() < v2.squared_length()) {
		v1_bigger = false;
	}

	//Left
	if (v1_bigger && v1.squared_length() >= this->length*this->length) {
		split_left(p3, ress, p2, p4, p1, v1);
		return ress;
	}
	if (!v1_bigger && v2.squared_length() >= this->length*this->length) {
		split_up(ress, p3, p1, p4, v2, p2);
		return ress;
	}
	if (v1_bigger && v2.squared_length() >= this->width*this->width){
		split_up(ress, p3, p1, p4, v2, p2);
		return ress;
	}

	if (!v1_bigger && v1.squared_length() >= this->width*this->width){
		split_left(p3, ress, p2, p4, p1, v1);
		return ress;
	}
	return ress;
}
