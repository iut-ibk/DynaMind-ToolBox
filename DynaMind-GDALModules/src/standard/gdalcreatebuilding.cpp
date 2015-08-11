#include "gdalcreatebuilding.h"

#include <ogr_api.h>
#include <ogrsf_frmts.h>


typedef CGAL::Point_2< SFCGAL::Kernel >              Point_2 ;
typedef CGAL::Vector_2< SFCGAL::Kernel >             Vector_2 ;
typedef CGAL::Polygon_2< SFCGAL::Kernel >            Polygon_2 ;
typedef CGAL::Polygon_with_holes_2< SFCGAL::Kernel > Polygon_with_holes_2;
typedef std::list<Polygon_with_holes_2>              Pwh_list_2;

DM_DECLARE_CUSTOM_NODE_NAME(GDALCreateBuilding,Generate Simple Buildings, Urban Form)

OGRGeometry* GDALCreateBuilding::createBuilding(OGRPolygon *ogr_poly)
{
	char* geo;
	ogr_poly->exportToWkt(&geo);
	std::auto_ptr<  SFCGAL::Geometry > g( SFCGAL::io::readWkt(geo));
	OGRFree(geo);

	SFCGAL::Polygon poly = g->as<SFCGAL::Polygon>();

	//Transfer to GDAL polygon
	Polygon_with_holes_2 p = poly.toPolygon_with_holes_2(true);

	Polygon_2 p_c;
	CGAL::convex_hull_2(p.outer_boundary().vertices_begin(), p.outer_boundary().vertices_end(), std::back_inserter(p_c));

	//Cacluate Minimal Rect
	Polygon_2 p_m;
	CGAL::min_rectangle_2(p_c.vertices_begin(), p_c.vertices_end(), std::back_inserter(p_m));


	Point_2 p1 = p_m.vertex(0);
	Point_2 p2 = p_m.vertex(1);

	Point_2 p3 = p_m.vertex(2);
	Point_2 p4 = p_m.vertex(3);

	Vector_2 v1 = (p2-p1);
	Vector_2 v2 = (p3-p2);

	bool v1_bigger = true;
	if (v1.squared_length() < v2.squared_length()) {
		v1_bigger = false;
	}


	Vector_2 e1 = v1 / sqrt(CGAL::to_double(v1.squared_length()));
	Vector_2 e2 = v2 / sqrt(CGAL::to_double(v2.squared_length()));

	OGRPoint ct;
	ogr_poly->Centroid(&ct);

	Point_2 centre(ct.getX(), ct.getY());

	double w = (!v1_bigger) ? this->width/2. : this->height/2.;
	double h = (!v1_bigger) ? this->height/2. : this->width/2.;

	Polygon_with_holes_2 footprint;
	footprint.outer_boundary().push_back( centre - e1*w - e2*h );
	footprint.outer_boundary().push_back( centre + e1*w - e2*h );
	footprint.outer_boundary().push_back( centre + e1*w + e2*h );
	footprint.outer_boundary().push_back( centre - e1*w + e2*h );

	SFCGAL::Polygon f(footprint);

	return addToSystem(f);


}

OGRGeometry *  GDALCreateBuilding::addToSystem(SFCGAL::Polygon & poly)
{

	std::string wkt = poly.asText(9).c_str();

	char * writable_wr = new char[wkt.size() + 1]; //Note not sure if memeory hole?
	std::copy(wkt.begin(), wkt.end(), writable_wr);
	writable_wr[wkt.size()] = '\0';

	OGRGeometry * ogr_poly;

	OGRErr err = OGRGeometryFactory::createFromWkt(&writable_wr, 0, &ogr_poly);

	if (!ogr_poly->IsValid()) {
		DM::Logger(DM::Warning) << "Geometry is not valid!";
		return 0;
	}
	if (ogr_poly->IsEmpty()) {
		DM::Logger(DM::Warning) << "Geometry is empty ";
		DM::Logger(DM::Warning) << "OGR Error " << err;
		DM::Logger(DM::Warning) << poly.asText(9);
		return 0;
	}

	return ogr_poly;
}

GDALCreateBuilding::GDALCreateBuilding()
{
	GDALModule = true;

	this->width = 10;
	this->addParameter("width", DM::DOUBLE, &this->width);

	this->height = 10;
	this->addParameter("length", DM::DOUBLE, &this->height);

	this->residential_units = 1;
	this->addParameter("residential_units", DM::INT, &this->residential_units);

	parcel = DM::ViewContainer("parcel", DM::FACE, DM::READ);
	building = DM::ViewContainer("building", DM::FACE, DM::WRITE);
	building.addAttribute("residential_units", DM::Attribute::INT, DM::WRITE);

	std::vector<DM::ViewContainer*> data_stream;
	data_stream.push_back(&parcel);
	data_stream.push_back(&building);

	this->registerViewContainers(data_stream);
}

void GDALCreateBuilding::run()
{
	parcel.resetReading();

	OGRFeature * f;

	while (f = parcel.getNextFeature()) {
		OGRPolygon * geo = (OGRPolygon *)f->GetGeometryRef();
		if (!geo)
			continue;
		OGRGeometry * building_geo  = this->createBuilding(geo);

		if (!building_geo)
			continue;
		//Create Feature
		OGRFeature * b = building.createFeature();
		b->SetGeometry(building_geo);
		b->SetField("residential_units", this->residential_units);
		OGRGeometryFactory::destroyGeometry(building_geo);
	}
}

string GDALCreateBuilding::getHelpUrl()
{
	return "/DynaMind-GDALModules/gdalcreatebuilding.html";
}
