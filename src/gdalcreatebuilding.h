#ifndef GDALCREATEBUILDING_H
#define GDALCREATEBUILDING_H

#include <dmmodule.h>
#include <dm.h>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/min_quadrilateral_2.h>
#include <CGAL/convex_hull_2.h>


#include <SFCGAL/MultiPolygon.h>

#include <SFCGAL/io/wkt.h>
#include <SFCGAL/algorithm/offset.h>

class DM_HELPER_DLL_EXPORT OGRPolygon;

class DM_HELPER_DLL_EXPORT GDALCreateBuilding : public DM::Module
{
		DM_DECLARE_NODE(GDALCreateBuilding)
private:

	DM::ViewContainer parcel;
	DM::ViewContainer building;

	double width;
	double height;
	int residential_units;
	OGRGeometry *createBuilding(OGRPolygon *ogr_poly);
	OGRGeometry *addToSystem(SFCGAL::Polygon &poly);
public:
	GDALCreateBuilding();
	void run();


};

#endif // GDALCREATEBUILDING_H
