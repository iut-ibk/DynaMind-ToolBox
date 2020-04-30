#include "scalepolygon.h"

#include <dmgdalsystem.h>


#include <ogrsf_frmts.h>

#include <iostream>
#include <geos_c.h>

#define CGAL_HAS_THREADS
DM_DECLARE_CUSTOM_NODE_NAME(ScalePolygon, Scale Polygon, Geometry Processing)

ScalePolygon::ScalePolygon()
{
	GDALModule = true;


	this->blockName = "cityblock";
	this->addParameter("blockName", DM::STRING, &this->blockName);

	this->subdevisionName = "parcel";
	this->addParameter("subdevisionName", DM::STRING, &this->subdevisionName);

	this->scale = 0;
	this->addParameter("scale", DM::DOUBLE, &this->scale);

	cityblocks = DM::ViewContainer(this->blockName, DM::FACE, DM::READ);
	parcels = DM::ViewContainer(this->subdevisionName, DM::FACE, DM::WRITE);

	std::vector<DM::ViewContainer*> views;
	views.push_back(&cityblocks);
	views.push_back(&parcels);

	this->registerViewContainers(views);

	counter_added = 0;
}

void ScalePolygon::init()
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


void ScalePolygon::run()
{
	cityblocks.resetReading();
	OGRFeature *poFeature;
	int counter = 0;

	while( (poFeature = cityblocks.getNextFeature()) != nullptr ) {
		counter++;
		OGRGeometry * ogr_poly = poFeature->GetGeometryRef();
		OGRPoint centroid;

		ogr_poly->Centroid(&centroid);

		double c_x = centroid.getX();
		double c_y = centroid.getY();

		OGRPolygon * poly =  ogr_poly->toPolygon();

		OGRLinearRing * r = poly->getExteriorRing();
		OGRPoint p;
		OGRLinearRing lr;
		for (int i = 0; i < r->getNumPoints(); i++){
			r->getPoint(i, &p);
			double x = p.getX();
			double y = p.getY();

			double dx = x - c_x;
			double dy = y - c_y;

			lr.addPoint(c_x + dx*scale, c_y + dy*scale);

		}
		OGRPolygon poly_scaled;
		poly_scaled.addRing(&lr);
		OGRFeature * feature = parcels.createFeature();
		feature->SetGeometry(&poly_scaled);
	}

	parcels.syncAlteredFeatures();
}


std::string ScalePolygon
::getHelpUrl()
{
	return "/DynaMind-GDALModules/gdaloffset.html";
}


