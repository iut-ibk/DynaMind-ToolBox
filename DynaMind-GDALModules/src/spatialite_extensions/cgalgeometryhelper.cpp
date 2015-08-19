#include "cgalgeometryhelper.h"

#define CGAL_HAS_THREADS

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/min_quadrilateral_2.h>
#include <CGAL/convex_hull_2.h>


#include <SFCGAL/MultiPolygon.h>

#include <SFCGAL/io/wkt.h>

typedef CGAL::Point_2< SFCGAL::Kernel >              Point_2 ;
typedef CGAL::Vector_2< SFCGAL::Kernel >             Vector_2 ;
typedef CGAL::Polygon_2< SFCGAL::Kernel >            Polygon_2 ;
typedef CGAL::Polygon_with_holes_2< SFCGAL::Kernel > Polygon_with_holes_2;
typedef std::list<Polygon_with_holes_2>              Pwh_list_2;


double CGALGeometryHelper::percentageFilled(const unsigned char * wkt_geo) {

	std::string str_wkt =std::string(reinterpret_cast<const char*>(wkt_geo));

	if (str_wkt.empty())
		return -1;

	std::auto_ptr<  SFCGAL::Geometry > g( SFCGAL::io::readWkt(str_wkt));
	SFCGAL::Polygon poly = g->as<SFCGAL::Polygon>();

	if (poly.isEmpty())
		return -2;

	Polygon_2 po = poly.toPolygon_2(false);
	if (!po.is_simple())
		return -3;

	Polygon_2 p_c;
	CGAL::convex_hull_2(po.vertices_begin(), po.vertices_end(), std::back_inserter(p_c));

	if (!p_c.is_simple())
		return -4;

	//Cacluate Minimal Rect
	Polygon_2 p_m;
	CGAL::min_rectangle_2(p_c.vertices_begin(), p_c.vertices_end(), std::back_inserter(p_m));
	double fraction = CGAL::to_double(p_c.area()) / CGAL::to_double(p_m.area());
	return fraction;
}

double CGALGeometryHelper::aspectRationBB(const unsigned char * wkt_geo) {

	std::string str_wkt =std::string(reinterpret_cast<const char*>(wkt_geo));

	if (str_wkt.empty())
		return -1;

	std::auto_ptr<  SFCGAL::Geometry > g( SFCGAL::io::readWkt(str_wkt));
	SFCGAL::Polygon poly = g->as<SFCGAL::Polygon>();

	if (poly.isEmpty())
		return -2;

	Polygon_2 po = poly.toPolygon_2(false);
	if (!po.is_simple())
		return -3;

	Polygon_2 p_c;
	CGAL::convex_hull_2(po.vertices_begin(), po.vertices_end(), std::back_inserter(p_c));

	if (!p_c.is_simple())
		return -4;

	//Cacluate Minimal Rect
	Polygon_2 p_m;
	CGAL::min_rectangle_2(p_c.vertices_begin(), p_c.vertices_end(), std::back_inserter(p_m));


	Point_2 p1 = p_m.vertex(0);
	Point_2 p2 = p_m.vertex(1);

	Point_2 p3 = p_m.vertex(2);

	Vector_2 v1 = (p2-p1);
	Vector_2 v2 = (p3-p2);

	double l1 = sqrt(CGAL::to_double(v1.squared_length()));
	double l2 = sqrt(CGAL::to_double(v2.squared_length()));

	double aspect_ration = l1/l2;

	if (aspect_ration < 1.0) {
		aspect_ration = 1.0/aspect_ration;
	}

	return aspect_ration;
}

