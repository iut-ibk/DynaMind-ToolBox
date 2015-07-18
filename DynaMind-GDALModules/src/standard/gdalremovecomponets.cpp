#include "gdalremovecomponets.h"

DM_DECLARE_CUSTOM_NODE_NAME(GDALRemoveComponets, Remove Component, Data Handling)
GDALRemoveComponets::GDALRemoveComponets()
{
	GDALModule = true;

	srand (time(NULL));

	this->viewName = "";
	this->addParameter("view_name", DM::STRING, &this->viewName);

	//dummy to get the ports
	std::vector<DM::ViewContainer> data;
	data.push_back(  DM::ViewContainer ("dummy", DM::SUBSYSTEM, DM::MODIFY) );
	this->addGDALData("city", data);

}

void GDALRemoveComponets::init()
{
	if (viewName.empty())
		return;
	this->vc = DM::ViewContainer(viewName, DM::FACE, DM::MODIFY);
	std::vector<DM::ViewContainer*> data_stream;
	data_stream.push_back(&vc);
	this->registerViewContainers(data_stream);
}

void GDALRemoveComponets::run()
{
	OGRFeature * f;
	vc.resetReading();
	while(f = vc.getNextFeature()) {
		vc.deleteFeature(f->GetFID());
	}
}

string GDALRemoveComponets::getHelpUrl()
{
	return "/DynaMind-GDALModules/gdalremovecomponents.html";
}
