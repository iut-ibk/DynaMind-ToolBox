#include "offsetworker.h"

#define CGAL_HAS_THREADS

OffsetWorker::OffsetWorker()
{
}

OffsetWorker::OffsetWorker(GDALOffset * module, char * wkt_poly, double offset) :
	module(module),
	offset(offset),
	wkt_poly(wkt_poly)
{

}

void OffsetWorker::run()
{
	std::auto_ptr<  SFCGAL::Geometry > g( SFCGAL::io::readWkt(wkt_poly));

	switch ( g->geometryTypeId() ) {
	case SFCGAL::TYPE_POLYGON:
		break;
	default:
		return;
	}
	SFCGAL::Polygon poly = g->as<SFCGAL::Polygon>();
	Polygon_2 p = poly.toPolygon_2(true);

	this->offsetPolygon(p, offset);
}

void OffsetWorker::offsetPolygon(Polygon_2 poly, double offset)  {
	typedef CGAL::Gps_circle_segment_traits_2<SFCGAL::Kernel>  Gps_traits_2;
	typedef Gps_traits_2::Polygon_2                            Offset_polygon_2;

	if(!poly.is_simple()) {
		DM::Logger(DM::Warning) << "Can't perform offset polygon is not simple";
	}
	CGAL::Orientation orient = poly.orientation();
	if (orient == CGAL::CLOCKWISE) {
		poly.reverse_orientation();
	}

	const double                           err_bound = 0.00001;
	std::list<Offset_polygon_2>            inset_polygons;

	CGAL::approximated_inset_2 (poly, offset, err_bound, std::back_inserter (inset_polygons));

	foreach (Offset_polygon_2 p, inset_polygons) {
		SFCGAL::Polygon po(this->approximate(p));
		QString wkt = QString(po.asText(9).c_str());
		module->addToSystem(wkt);
		//emit resultPolygon(wkt);
	}
}

Polygon_2 OffsetWorker::approximate( const Offset_polygon_2& polygon, const int& n )
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
