#include "parceling.h"
#include <dmgdalsystem.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/point_generators_2.h>
#include <CGAL/random_convex_set_2.h>
#include <CGAL/min_quadrilateral_2.h>
#include <iostream>
#include <SFCGAL/MultiPolygon.h>

#include <SFCGAL/io/wkt.h>
#include <SFCGAL/algorithm/offset.h>

//typedef CGAL::Simple_cartesian<double>            Kernel;
//typedef Kernel::Point_2                           Point_2;
//typedef Kernel::Line_2                            Line_2;
//typedef CGAL::Polygon_2<Kernel>                   Polygon_2;
//typedef CGAL::Random_points_in_square_2<Point_2>  Generator;


typedef CGAL::Polygon_2< SFCGAL::Kernel >            Polygon_2 ;
typedef CGAL::Polygon_with_holes_2< SFCGAL::Kernel > Polygon_with_holes_2 ;


DM_DECLARE_NODE_NAME(GDALParceling, GDALModules)



GDALParceling::GDALParceling()
{
	GDALModule = true;

	cityblocks = DM::ViewContainer("CITYBLOCK", DM::FACE, DM::READ);

	parcels = DM::ViewContainer("PARCEL", DM::FACE, DM::WRITE);

	std::vector<DM::ViewContainer> views;
	views.push_back(cityblocks);
	views.push_back(parcels);

	this->addGDALData("city", views);

}


void GDALParceling::run()
{
	DM::GDALSystem * city = this->getGDALData("city");

	cityblocks.setCurrentGDALSystem(city);
	parcels.setCurrentGDALSystem(city);

	cityblocks.resetReading();
	OGRFeature *poFeature;
	int counter = 0;
	while( (poFeature = cityblocks.getNextFeature()) != NULL ) {
		counter++;
		char* geo;

		poFeature->GetGeometryRef()->exportToWkt(&geo);
		std::auto_ptr<  SFCGAL::Geometry > g( SFCGAL::io::readWkt(geo));
		std::cout << g->numGeometries();

		switch ( g->geometryTypeId() ) {
			case SFCGAL::TYPE_POLYGON:
				std::cout << "ploy" << std::endl;
				break;
			default:
				std::cout << "unkown" << std::endl;
				continue;
			}
		SFCGAL::Polygon poly = g->as<SFCGAL::Polygon>();
		Polygon_2 p = poly.toPolygon_2(true);
		Polygon_2 p_m;
		CGAL::min_rectangle_2(p.vertices_begin(), p.vertices_end(), std::back_inserter(p_m));
		std::cout << p_m << std::endl;
		SFCGAL::Polygon geom(p_m);

		OGRFeature * parcel = parcels.createFeature();
		OGRPolygon ogr_poly;

		const char* mstr = geom.asText().c_str();
		char* str=const_cast<char*>(mstr);

		ogr_poly.importFromWkt(&str);


		parcel->SetGeometry(&ogr_poly);



	}

	parcels.syncAlteredFeatures();
}





