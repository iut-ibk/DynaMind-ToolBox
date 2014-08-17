#include "gdalgeometricattributes.h"
#include <ogr_api.h>
#include <ogrsf_frmts.h>

DM_DECLARE_NODE_NAME(GDALGeometricAttributes, GDALModules)

GDALGeometricAttributes::GDALGeometricAttributes()
{
	GDALModule = true;

	this->leadingViewName = "";
	this->addParameter("leading_view", DM::STRING, &this->leadingViewName);

	this->isCalculateArea = true;
	this->addParameter("calculate_area", DM::BOOL, &this->isCalculateArea);
}

void GDALGeometricAttributes::init()
{
	if (leadingViewName.empty())
		return;
	vc = DM::ViewContainer(this->leadingViewName, DM::FACE, DM::READ);
	if (isCalculateArea)
		vc.addAttribute("area", DM::Attribute::DOUBLE, DM::WRITE);

	std::vector<DM::ViewContainer*> data_stream;
	data_stream.push_back(&vc);

	this->registerViewContainers(data_stream);
}

void GDALGeometricAttributes::run()
{
	OGRFeature * f;

	vc.resetReading();

	while( f = vc.getNextFeature() ) {
		if (isCalculateArea) {
			OGRPolygon* geo = (OGRPolygon*)f->GetGeometryRef();
			f->SetField("area", this->calculateArea( geo ));
		}
	}

}

double GDALGeometricAttributes::calculateArea(OGRPolygon * poly) {
	return poly->get_Area();
}

