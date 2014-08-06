#include "gdalspatiallinking.h"
#include <sstream>
#include <ogr_feature.h>

DM_DECLARE_NODE_NAME(GDALSpatialLinking, GDALModules)


GDALSpatialLinking::GDALSpatialLinking()
{
	GDALModule = true;
	properInit = false;

	this->leadingViewName = "";
	this->addParameter("leadingViewName", DM::STRING, &leadingViewName);

	this->linkViewName = "";
	this->addParameter("linkViewName", DM::STRING, &linkViewName);

	//dummy to get the ports
	std::vector<DM::ViewContainer> data;
	data.push_back(  DM::ViewContainer ("dummy", DM::SUBSYSTEM, DM::MODIFY) );
	this->addGDALData("city", data);
}

void GDALSpatialLinking::init()
{
	properInit = false;
	if (this->leadingViewName.empty())
		return;
	if (this->leadingViewName.empty())
		return;
	std::map<std::string, DM::View> inViews = getViewsInStream()["city"];
	if (inViews.find(leadingViewName) == inViews.end()) {
		DM::Logger(DM::Error) << "view " << leadingViewName << " not found in in stream";
		return;
	}
	if (inViews.find(linkViewName) == inViews.end()) {
		DM::Logger(DM::Error) << "view " << linkViewName << " not found in in stream";
		return;
	}
	this->leadingView = DM::ViewContainer(leadingViewName, inViews[leadingViewName].getType(), DM::READ);

	this->linkView = DM::ViewContainer(linkViewName, inViews[linkViewName].getType(), DM::READ);
	std::stringstream l_name;
	l_name << leadingViewName << "_id";
	link_name = l_name.str();
	this->linkView.addAttribute(link_name, DM::Attribute::INT, DM::WRITE);

	std::vector<DM::ViewContainer *> data_stream;
	data_stream.push_back(&leadingView);
	data_stream.push_back(&linkView);

	this->registerViewContainers(data_stream);

	properInit = true;
}

void GDALSpatialLinking::run()
{
	if (!properInit) {
		DM::Logger(DM::Error) << "GDALSpatialLinking init failed";
		return;
	}
	leadingView.resetReading();

	OGRFeature * lead_feat = 0;
	while (lead_feat = leadingView.getNextFeature()) {

		OGRFeature * link_feature = 0;
		OGRGeometry * lead_geo = lead_feat->GetGeometryRef();
		int id = lead_feat->GetFID();

		linkView.resetReading();
		linkView.setSpatialFilter(lead_geo);
		while (link_feature = linkView.getNextFeature()) {
			OGRPoint ct;
			link_feature->GetGeometryRef()->Centroid(&ct);
			if (ct.Within(lead_geo)) {
				link_feature->SetField(link_name.c_str(), id);
			}

		}
	}
}
