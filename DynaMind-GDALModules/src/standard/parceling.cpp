#include "parceling.h"
#include <dmgdalsystem.h>

#include <ogrsf_frmts.h>

#include <iostream>

#include <CGAL/Boolean_set_operations_2.h>
#include <SFCGAL/algorithm/intersection.h>

#include <CGAL/Boolean_set_operations_2.h>
#include <CGAL/intersections.h>

DM_DECLARE_CUSTOM_NODE_NAME(GDALParceling,Parcel Faces, Urban Form)

GDALParceling::GDALParceling()
{
	GDALModule = true;

	this->width = 100;
	this->addParameter("width", DM::DOUBLE, &this->width);

	this->length = 100;
	this->addParameter("length", DM::DOUBLE, &this->length);

	this->blockName = "cityblock";
	this->addParameter("blockName", DM::STRING, &this->blockName);

	this->subdevisionName = "parcel";
	this->addParameter("subdevisionName", DM::STRING, &this->subdevisionName);

	cityblocks = DM::ViewContainer(this->blockName, DM::FACE, DM::READ);
	parcels = DM::ViewContainer(this->subdevisionName, DM::FACE, DM::WRITE);

	std::vector<DM::ViewContainer*> views;
	views.push_back(&cityblocks);
	views.push_back(&parcels);

	this->registerViewContainers(views);

	counter_added = 0;

}

void GDALParceling::addToSystem(SFCGAL::Polygon & poly)
{

	std::string wkt = poly.asText(9).c_str();

	char * writable_wr = new char[wkt.size() + 1]; //Note not sure if memeory hole?
	std::copy(wkt.begin(), wkt.end(), writable_wr);
	writable_wr[wkt.size()] = '\0';

	OGRGeometry * ogr_poly;

	OGRErr err = OGRGeometryFactory::createFromWkt(&writable_wr, 0, &ogr_poly);

	if (!ogr_poly->IsValid()) {
		DM::Logger(DM::Warning) << "Geometry is not valid!";
		return;
	}
	if (ogr_poly->IsEmpty()) {
		DM::Logger(DM::Warning) << "Geometry is empty ";
		DM::Logger(DM::Warning) << "OGR Error " << err;
		DM::Logger(DM::Warning) << poly.asText(9);
		return;
	}
	//Create Feature
	OGRFeature * parcel = parcels.createFeature();
	parcel->SetGeometry(ogr_poly);
	OGRGeometryFactory::destroyGeometry(ogr_poly);

	counter_added++;
}

void GDALParceling::split_left(Point_2 & p3, Pwh_list_2 & ress, Point_2 & p2, Point_2 & p4, Point_2 & p1, Vector_2 & v1)
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

void GDALParceling::split_up(Pwh_list_2 & ress, Point_2 & p3, Point_2 & p1, Point_2 & p4, Vector_2 & v2, Point_2 & p2)
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

Pwh_list_2 GDALParceling::splitter(Polygon_2 &rect)
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
	parcels.syncAlteredFeatures();
	DM::Logger(DM::Debug) << this->counter_added;
}

void GDALParceling::init()
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

string GDALParceling::getHelpUrl()
{
	return "/DynaMind-GDALModules/gdalparceling.html";
}

void GDALParceling::splitePoly(Polygon_with_holes_2 &p)
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
		addToSystem(split_geo);
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


