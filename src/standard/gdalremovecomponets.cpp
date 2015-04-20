#include "gdalremovecomponets.h"

DM_DECLARE_NODE_NAME(GDALRemoveComponets, GDALModules)

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
	DM::Logger(DM::Error) << "filter";
	std::vector<DM::Filter> filters = this->getFilter();
	foreach (DM::Filter fl, filters) {
		DM::Logger(DM::Error) << "filter:" << fl.getAttributeFilter().getArgument();
	}
}

void GDALRemoveComponets::run()
{
	OGRFeature * f;
	vc.resetReading();
	std::vector<DM::Filter> filters = this->getFilter();

	foreach (DM::Filter fl, filters) {
		DM::Logger(DM::Error) << "filter:" << fl.getAttributeFilter().getArgument();
	}

	while(f = vc.getNextFeature()) {
		vc.deleteFeature(f->GetFID());
	}
}
