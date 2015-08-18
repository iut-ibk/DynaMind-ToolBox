#ifndef GEOMETRICATTRIBUTEWORKER_H
#define GEOMETRICATTRIBUTEWORKER_H

#include <ogrsf_frmts.h>

#include <QRunnable>
#define CGAL_HAS_THREADS

#include "gdalgeometricattributes.h"

class GeometricAttributeWorker:  public  QRunnable
{
private:
	int ogc_fid;
	std::vector<OGRFeature*> features;
	GDALGeometricAttributes * callback;
	bool isCalculateArea;
	bool isAspectRationBB;
	bool isPercentageFilled;
	double calculateArea(OGRPolygon *poly);
	double percentageFilled(OGRPolygon *ogr_poly);
	double aspectRationBB(OGRPolygon *ogr_poly);
public:
	GeometricAttributeWorker() {}
	GeometricAttributeWorker(GDALGeometricAttributes * callback,
							std::vector<OGRFeature*>  features,
							bool isCalculateArea,
							bool isAspectRationBB,
							bool isPercentageFilled);
	void run();
};

#endif // GEOMETRICATTRIBUTEWORKER_H
