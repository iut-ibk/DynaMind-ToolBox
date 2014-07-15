#ifndef PARCELING_H
#define PARCELING_H

#include <dmmodule.h>
#include <dm.h>
#include <dmviewcontainer.h>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/min_quadrilateral_2.h>
#include <CGAL/convex_hull_2.h>


#include <SFCGAL/MultiPolygon.h>

#include <SFCGAL/io/wkt.h>
#include <SFCGAL/algorithm/offset.h>
//#include <SFCGAL/algorithm/intersection.h>

typedef CGAL::Point_2< SFCGAL::Kernel >              Point_2 ;
typedef CGAL::Vector_2< SFCGAL::Kernel >             Vector_2 ;
typedef CGAL::Polygon_2< SFCGAL::Kernel >            Polygon_2 ;
typedef CGAL::Polygon_with_holes_2< SFCGAL::Kernel > Polygon_with_holes_2;
typedef std::list<Polygon_with_holes_2>                   Pwh_list_2;


class DM_HELPER_DLL_EXPORT GDALParceling: public DM::Module
{
	DM_DECLARE_NODE(GDALParceling)
private:
	DM::ViewContainer cityblocks;
	DM::ViewContainer parcels;
	int counter_added;
	double length;
	double width;


public:
	GDALParceling();
	void run();

	void splitePoly(Polygon_with_holes_2 &poly);



	void addToSystem(SFCGAL::Polygon &poly);
	Pwh_list_2 splitter(Polygon_2 & rect);
	void split_left(Point_2 &p3, Pwh_list_2 &ress, Point_2 &p2, Point_2 &p4, Point_2 &p1, Vector_2 &v1);
	void split_up(Pwh_list_2 &ress, Point_2 &p3, Point_2 &p1, Point_2 &p4, Vector_2 &v2, Point_2 &p2);
};

#endif // PARCELING_H
