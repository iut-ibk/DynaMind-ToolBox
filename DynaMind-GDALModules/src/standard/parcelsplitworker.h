#ifndef PARCELSPLITWORKER_H
#define PARCELSPLITWORKER_H

#include <QRunnable>
#define CGAL_HAS_THREADS

#include <dmmodule.h>
#include <dm.h>
#include <dmviewcontainer.h>

#include "gdalparcelsplit.h"

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/min_quadrilateral_2.h>
#include <CGAL/convex_hull_2.h>


#include <SFCGAL/MultiPolygon.h>

#include <SFCGAL/io/wkt.h>
#include <SFCGAL/algorithm/offset.h>


typedef CGAL::Point_2< SFCGAL::Kernel >              Point_2 ;
typedef CGAL::Vector_2< SFCGAL::Kernel >             Vector_2 ;
typedef CGAL::Polygon_2< SFCGAL::Kernel >            Polygon_2 ;
typedef CGAL::Polygon_with_holes_2< SFCGAL::Kernel > Polygon_with_holes_2;
typedef std::list<Polygon_with_holes_2>              Pwh_list_2;


class DM_HELPER_DLL_EXPORT ParcelSplitWorker : public QObject, public  QRunnable
{
	Q_OBJECT
private:
	double width;
	bool splitFirst;
	char * poly_wkt;
	GDALParcelSplit * module;

public:
	ParcelSplitWorker();
	ParcelSplitWorker(GDALParcelSplit * module, double width, bool splitFirst,char * poly_wkt);
	void splitePoly(Polygon_with_holes_2 &poly);
	Pwh_list_2 splitter(Polygon_2 & rect);
	void run();
	~ParcelSplitWorker(){}

signals:
	void resultPolygon(QString s);
};

#endif // PARCELSPLITWORKER_H
