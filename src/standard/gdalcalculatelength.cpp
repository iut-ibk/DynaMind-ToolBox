#include "gdalcalculatelength.h"
#include <ogr_api.h>
#include <ogrsf_frmts.h>

DM_DECLARE_NODE_NAME(GDALCalculateLength, GDALModules)

GDALCalculateLength::GDALCalculateLength()
{
	GDALModule = true;

	this->leadingViewName = "";
	this->addParameter("leading_view", DM::STRING, &this->leadingViewName);

}

void GDALCalculateLength::init()
{
	if (leadingViewName.empty())
		return;
	vc = DM::ViewContainer(this->leadingViewName, DM::EDGE, DM::READ);
	vc.addAttribute("length", DM::Attribute::DOUBLE, DM::WRITE);

	std::vector<DM::ViewContainer*> data_stream;
	data_stream.push_back(&vc);

	this->registerViewContainers(data_stream);
}

void GDALCalculateLength::run()
{
	OGRFeature * f;
	vc.resetReading();
	while( f = vc.getNextFeature() ) {
		OGRLineString* geo = (OGRLineString*)f->GetGeometryRef();
		f->SetField("length", geo->get_Length());
	}
}
