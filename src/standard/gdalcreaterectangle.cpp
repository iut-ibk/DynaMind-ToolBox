#include "gdalcreaterectangle.h"
#include "ogrsf_frmts.h"
#include "dmgdalsystem.h"
DM_DECLARE_NODE_NAME(GDALCreateRectangle, GDALModules)

GDALCreateRectangle::GDALCreateRectangle()
{
	GDALModule = true;

	this->viewName = "";
	this->addParameter("view_name", DM::STRING, &this->viewName);

	this->width = 100;
	this->addParameter("width", DM::DOUBLE, &this->width);

	this->length = 100;
	this->addParameter("height", DM::DOUBLE, &this->length);
}

void GDALCreateRectangle::init()
{
	if (this->viewName.empty())
		return;
	view = DM::ViewContainer(this->viewName, DM::FACE, DM::WRITE);

	std::vector<DM::ViewContainer*> datastream;
	datastream.push_back(&view);

	this->registerViewContainers(datastream);
}

void GDALCreateRectangle::run() {

	OGRLinearRing lr;
	lr.addPoint(0,0,0);
	lr.addPoint(width,0,0);
	lr.addPoint(width, length,0);
	lr.addPoint(0, length, 0);
	lr.addPoint(0, 0, 0);
	OGRPolygon poly;
	poly.addRing(&lr);

	OGRFeature * feature = view.createFeature();

	feature->SetGeometry(&poly);


}
