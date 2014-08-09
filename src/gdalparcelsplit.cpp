#include "gdalparcelsplit.h"
#include <dmgdalsystem.h>

#include <ogrsf_frmts.h>

#include <iostream>

#include <CGAL/Boolean_set_operations_2.h>
#include <SFCGAL/algorithm/intersection.h>

#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/intersections.h>




DM_DECLARE_NODE_NAME(GDALParcelSplit, GDALModules)

GDALParcelSplit::GDALParcelSplit()
{
	GDALModule = true;

	this->width = 100;
	this->addParameter("width", DM::DOUBLE, &this->width);

	this->length = 100;
	this->addParameter("length", DM::DOUBLE, &this->length);

	this->blockName = "CITYBLOCK";
	this->addParameter("blockName", DM::STRING, &this->blockName);

	this->subdevisionName = "PARCEL";
	this->addParameter("subdevisionName", DM::STRING, &this->subdevisionName);

	cityblocks = DM::ViewContainer(this->blockName, DM::FACE, DM::READ);
	parcels = DM::ViewContainer(this->subdevisionName, DM::FACE, DM::WRITE);

	std::vector<DM::ViewContainer*> views;
	views.push_back(&cityblocks);
	views.push_back(&parcels);

	this->registerViewContainers(views);

	counter_added = 0;

}

void GDALParcelSplit::init()
{
	if (this->blockName.empty() || this->subdevisionName.empty())
		return;

	cityblocks = DM::ViewContainer(this->blockName, DM::FACE, DM::READ);
	parcels = DM::ViewContainer(this->subdevisionName, DM::FACE, DM::WRITE);

	std::vector<DM::ViewContainer*> views;
	views.push_back(&cityblocks);
	views.push_back(&parcels);

	this->registerViewContainers(views);
}


void GDALParcelSplit::run()
{
	cityblocks.resetReading();
	OGRFeature *poFeature;
	int counter = 0;
	while( (poFeature = cityblocks.getNextFeature()) != NULL ) {
		counter++;
		char* geo;

		poFeature->GetGeometryRef()->exportToWkt(&geo);
		std::auto_ptr<  SFCGAL::Geometry > g( SFCGAL::io::readWkt(geo));

		switch ( g->geometryTypeId() ) {
		case SFCGAL::TYPE_POLYGON:
			break;
		default:
			continue;
		}
		SFCGAL::Polygon poly = g->as<SFCGAL::Polygon>();

		//Transfer to GDAL polygon
		Polygon_with_holes_2 p = poly.toPolygon_with_holes_2(true);
		splitePoly(p);
	}
	DM::Logger(DM::Error) << this->counter_added;
}


void GDALParcelSplit::splitePoly(Polygon_with_holes_2 &p)
{
	//Convex Hull (GDAL algorithm doesn't work without)
	Polygon_2 p_c;
	CGAL::convex_hull_2(p.outer_boundary().vertices_begin(), p.outer_boundary().vertices_end(), std::back_inserter(p_c));

	//Cacluate Minimal Rect
	Polygon_2 p_m;
	CGAL::min_rectangle_2(p_c.vertices_begin(), p_c.vertices_end(), std::back_inserter(p_m));

	Pwh_list_2 splitters(this->splitter(p_m));

//	if (splitters.size() == 0) {
//		SFCGAL::Polygon split_geo(p);
//		addToSystem(split_geo);
//		return;
//	}

	foreach (Polygon_with_holes_2 pwh, splitters) {
		Pwh_list_2 split_ress;
		CGAL::intersection(p, pwh,  std::back_inserter(split_ress));
		foreach(Polygon_with_holes_2 pwh_split, split_ress) {
			SFCGAL::Polygon split_geo(pwh_split);
			addToSystem(split_geo);
			//splitePoly(pwh_split);
		}
	}
	return;
}

void GDALParcelSplit::addToSystem(SFCGAL::Polygon & poly)
{

	std::string wkt = poly.asText(9).c_str();

	char * writable_wr = new char[wkt.size() + 1]; //Note not sure if memeory hole?
	std::copy(wkt.begin(), wkt.end(), writable_wr);
	writable_wr[wkt.size()] = '\0';

	OGRGeometry * ogr_poly;

	OGRErr err = OGRGeometryFactory::createFromWkt(&writable_wr, 0, &ogr_poly);

	if (!ogr_poly->IsValid()) {
		DM::Logger(DM::Error) << "Geometry is not valid!";
		return;
	}
	if (ogr_poly->IsEmpty()) {
		DM::Logger(DM::Error) << "Geometry is empty ";
		DM::Logger(DM::Error) << "OGR Error " << err;
		DM::Logger(DM::Error) << poly.asText(9);
		return;
	}
	//Create Feature
	OGRFeature * parcel = parcels.createFeature();
	parcel->SetGeometry(ogr_poly);
	OGRGeometryFactory::destroyGeometry(ogr_poly);

	counter_added++;
}


Pwh_list_2 GDALParcelSplit::splitter(Polygon_2 &rect)
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

	//Splitt Polygon on the short side in half and the rest in small peaces of 15m

	int numberOfElements_x = sqrt(CGAL::to_double(v1.squared_length()))/15.;
	int numberOfElements_y = 2;

	Vector_2 dv1 = v1/numberOfElements_x;
	Vector_2 dv2 = v2/numberOfElements_y;

	for (int dx = 0; dx < numberOfElements_x; dx++) {
		Polygon_with_holes_2 split_left_1;
		Vector_2 offsetx = dv1*dx;
		split_left_1.outer_boundary().push_back(p1+ offsetx);
		split_left_1.outer_boundary().push_back( p1+dv1 + offsetx);
		split_left_1.outer_boundary().push_back(p1+dv1+dv2 + offsetx);
		split_left_1.outer_boundary().push_back(p1+dv2 + offsetx);
		ress.push_back(split_left_1);
	}
	return ress;
}



