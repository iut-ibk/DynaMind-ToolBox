#include "gdalcopyelementtoview.h"

DM_DECLARE_NODE_NAME(GDALCopyElementToView, GDALModules)

typedef std::map<std::string, DM::View> viewmap;

GDALCopyElementToView::GDALCopyElementToView()
{
	GDALModule = true;

	fromViewName = "";
	this->addParameter("from_view", DM::STRING, &fromViewName);

	toViewName = "";
	this->addParameter("to_view", DM::STRING, &toViewName);

	//dummy to get the ports
	std::vector<DM::ViewContainer> data;
	data.push_back(  DM::ViewContainer ("dummy", DM::SUBSYSTEM, DM::MODIFY) );
	this->addGDALData("city", data);
}

void GDALCopyElementToView::init()
{
	if (fromViewName.empty() || toViewName.empty()) {
		DM::Logger(DM::Error) << "from or to view name not defined";
		return;
	}
	viewmap inViews = getViewsInStream()["city"];


	if (inViews.find(fromViewName) == inViews.end()) {
		DM::Logger(DM::Error) << fromViewName << " not found";
		return;
	}

	this->vc_from = DM::ViewContainer(fromViewName, inViews[fromViewName].getType(),DM::READ );
	this->vc_to = DM::ViewContainer(toViewName, inViews[fromViewName].getType(),DM::WRITE );


	std::vector<DM::ViewContainer*> data_stream;
	data_stream.push_back(&vc_from);
	data_stream.push_back(&vc_to);

	this->registerViewContainers(data_stream);
}

void GDALCopyElementToView::run()
{
	this->vc_from.resetReading();

	OGRFeature * f;
	while (f = vc_from.getNextFeature()) {
		OGRGeometry * geo = f->GetGeometryRef();
		if (!geo)
			continue;
		OGRFeature * f_new = vc_to.createFeature();
		f_new->SetGeometry(geo);
	}
}
