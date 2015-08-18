#ifndef OFFSETWORKER_H
#define OFFSETWORKER_H

#include <dmmodule.h>
#include <dm.h>
#define CGAL_HAS_THREADS
#include <dmviewcontainer.h>
#include "gdaloffset.h"

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Gps_circle_segment_traits_2.h>
#include <CGAL/approximated_offset_2.h>

#include <SFCGAL/MultiPolygon.h>
#include <SFCGAL/io/wkt.h>

typedef CGAL::Point_2< SFCGAL::Kernel >              Point_2 ;
typedef CGAL::Vector_2< SFCGAL::Kernel >             Vector_2 ;
typedef CGAL::Polygon_2< SFCGAL::Kernel >            Polygon_2 ;
typedef CGAL::Gps_circle_segment_traits_2<SFCGAL::Kernel>	Gps_traits_2;
typedef Gps_traits_2::Polygon_2								Offset_polygon_2;


class OffsetWorker : public QObject, public  QRunnable
{
	Q_OBJECT
private:
	double offset;
	char * wkt_poly;
	GDALOffset * module;
public:
	OffsetWorker();
	OffsetWorker(GDALOffset * module, char *poly, double offset);

	~OffsetWorker(){}
	void run();

	void offsetPolygon(Polygon_2 poly, double offset);
	Polygon_2 approximate(const Offset_polygon_2 &polygon, const int &n = 0);

signals:
	void resultPolygon(QString s);
};

#endif // OFFSETWORKER_H
