#include "gdaloffset.h"

#include "parceling.h"
#include <dmgdalsystem.h>

#include <ogrsf_frmts.h>

#include <iostream>


DM_DECLARE_NODE_NAME(GDALOffset, GDALModules)

GDALOffset::GDALOffset()
{
	GDALModule = true;

	this->blockName = "cityblock";
	this->addParameter("blockName", DM::STRING, &this->blockName);

	this->subdevisionName = "parcel";
	this->addParameter("subdevisionName", DM::STRING, &this->subdevisionName);

	this->offset = 0;
	this->addParameter("offset", DM::DOUBLE, &this->offset);

	cityblocks = DM::ViewContainer(this->blockName, DM::FACE, DM::READ);
	parcels = DM::ViewContainer(this->subdevisionName, DM::FACE, DM::WRITE);

	std::vector<DM::ViewContainer*> views;
	views.push_back(&cityblocks);
	views.push_back(&parcels);

	this->registerViewContainers(views);

	counter_added = 0;
}

void GDALOffset::addToSystem(SFCGAL::Polygon & poly)
{
	std::string wkt = poly.asText(9).c_str();

	char * writable_wr = new char[wkt.size() + 1]; //Note not sure if memory hole?
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


void GDALOffset::run()
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
		Polygon_2 p = poly.toPolygon_2(true);

		std::list<Polygon_2> results = this->offsetPolygon(p, this->offset);
		foreach(Polygon_2 poly, results) {
			SFCGAL::Polygon offest_poly(poly);
			this->addToSystem(offest_poly);
		}
	}
}

std::list<Polygon_2> GDALOffset::offsetPolygon(Polygon_2 poly, double offset)  {
	typedef CGAL::Gps_circle_segment_traits_2<SFCGAL::Kernel>  Gps_traits_2;
	typedef Gps_traits_2::Polygon_2                            Offset_polygon_2;
	std::list<Polygon_2> offset_polygons;
	if(!poly.is_simple()) {
		DM::Logger(DM::Warning) << "Can't perform offset polygon is not simple";
		return offset_polygons;
	}
	CGAL::Orientation orient = poly.orientation();
	if (orient == CGAL::CLOCKWISE) {
		poly.reverse_orientation();
	}

	const double                           err_bound = 0.00001;
	std::list<Offset_polygon_2>            inset_polygons;

	CGAL::approximated_inset_2 (poly, offset, err_bound, std::back_inserter (inset_polygons));
	std::list<Polygon_2> p_list;
	foreach (Offset_polygon_2 p, inset_polygons) {
		p_list.insert(p_list.begin(), this->approximate(p));
	}
	return p_list;
}

Polygon_2 GDALOffset::approximate( const Offset_polygon_2& polygon, const int& n )
{
	std::list<std::pair<double, double> > pair_list;

	/*
	 * iterate X_monotone_curve_2 components
	 */
	for ( Offset_polygon_2::Curve_const_iterator it = polygon.curves_begin();
		  it != polygon.curves_end(); ++it ) {
		it->approximate( std::back_inserter( pair_list ), n ) ;
	}

	// remove duplicated last point
	pair_list.pop_back() ;

	/*
	 * convertr to polygon
	 */
	Polygon_2 result ;

	bool isFirst = true ;
	SFCGAL::Kernel::Point_2 last ;
	SFCGAL::Kernel::Point_2 first ;

	for ( std::list<std::pair<double, double> >::const_iterator it = pair_list.begin(); it != pair_list.end(); ++it ) {
		SFCGAL::Kernel::Point_2 point( it->first, it->second ) ;

		if ( isFirst ) {
			first = point;
			isFirst = false ;
		}
		else if ( point == last ) {
			continue ;
		}

		result.push_back( point ) ;
		last = point ;
	}
	result.push_back( first );
	return result ;
}

void GDALOffset::init()
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

