#include "gdalgeometricattributes.h"
#include <ogr_api.h>
#include "geometricattributeworker.h"

DM_DECLARE_CUSTOM_NODE_NAME(GDALGeometricAttributes, Calculate Feature Area, Data Handling)

GDALGeometricAttributes::GDALGeometricAttributes()
{
	GDALModule = true;

	this->leadingViewName = "";
	this->addParameter("leading_view", DM::STRING, &this->leadingViewName);

	this->isCalculateArea = true;
	this->addParameter("calculate_area", DM::BOOL, &this->isCalculateArea);

	this->isAspectRationBB = false;
	this->addParameter("aspect_ratio_BB", DM::BOOL, &this->isAspectRationBB);

	this->isPercentageFilled = false;
	this->addParameter("percentage_filled", DM::BOOL, &this->isPercentageFilled);

}

void GDALGeometricAttributes::init()
{
	if (leadingViewName.empty())
		return;
	vc = DM::ViewContainer(this->leadingViewName, DM::FACE, DM::READ);
	if (isCalculateArea)
		vc.addAttribute("area", DM::Attribute::DOUBLE, DM::WRITE);
	if (isAspectRationBB)
		vc.addAttribute("aspect_ratio_BB", DM::Attribute::DOUBLE, DM::WRITE);
	if (isAspectRationBB)
		vc.addAttribute("percentage_filled", DM::Attribute::DOUBLE, DM::WRITE);

	std::vector<DM::ViewContainer*> data_stream;
	data_stream.push_back(&vc);

	this->registerViewContainers(data_stream);
}

string GDALGeometricAttributes::getHelpUrl()
{
	return "/DynaMind-GDALModules/gdalgeometricattributes.html";
}

void GDALGeometricAttributes::run()
{
	OGRFeature * f;

	vc.resetReading();
	QThreadPool pool;
	std::vector<OGRFeature*> container;
	int counter = 0;
	while( f = vc.getNextFeature() ) {
		container.push_back(f);
		if (counter%10000 == 0){
			GeometricAttributeWorker * gw = new GeometricAttributeWorker(this, container, isCalculateArea, isAspectRationBB , isPercentageFilled);
			pool.start(gw);
			container = std::vector<OGRFeature*>();
		}
	}
	pool.waitForDone();
}

