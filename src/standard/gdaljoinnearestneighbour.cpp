#include "gdaljoinnearestneighbour.h"
#include <sstream>
#include <ogr_feature.h>
#include <ogrsf_frmts.h>

DM_DECLARE_NODE_NAME(GDALJoinNearestNeighbour, GDALModules)


GDALJoinNearestNeighbour::GDALJoinNearestNeighbour()
{
	GDALModule = true;
	properInit = false;

	max_distance = 100;

	this->leadingViewName = "";
	this->addParameter("leadingViewName", DM::STRING, &leadingViewName);

	this->linkViewName = "";
	this->addParameter("linkViewName", DM::STRING, &linkViewName);

	this->max_distance = 100;
	this->addParameter("max_distance", DM::DOUBLE, &max_distance);

	//dummy to get the ports
	std::vector<DM::ViewContainer> data;
	data.push_back(  DM::ViewContainer ("dummy", DM::SUBSYSTEM, DM::MODIFY) );
	this->addGDALData("city", data);
}

void GDALJoinNearestNeighbour::init()
{
	properInit = false;
	if (this->leadingViewName.empty())
		return;
	if (this->linkViewName.empty())
		return;
	std::map<std::string, DM::View> inViews = getViewsInStream()["city"];
	if (inViews.find(leadingViewName) == inViews.end()) {
		DM::Logger(DM::Warning) << "view " << leadingViewName << " not found in in stream";
		return;
	}
	if (inViews.find(linkViewName) == inViews.end()) {
		DM::Logger(DM::Warning) << "view " << linkViewName << " not found in in stream";
		return;
	}
	this->leadingView = DM::ViewContainer(leadingViewName, inViews[leadingViewName].getType(), DM::READ);

	this->linkView = DM::ViewContainer(linkViewName, inViews[linkViewName].getType(), DM::READ);
	std::stringstream l_name;
	l_name << linkViewName << "_id";
	link_name = l_name.str();
	this->leadingView.addAttribute(link_name, linkViewName , DM::WRITE);

	std::vector<DM::ViewContainer *> data_stream;
	data_stream.push_back(&leadingView);
	data_stream.push_back(&linkView);

	this->registerViewContainers(data_stream);

	properInit = true;
}

void GDALJoinNearestNeighbour::run()
{
	if (!properInit) {
		DM::Logger(DM::Error) << "GDALSpatialLinking init failed";
		this->setStatus(DM::MOD_CHECK_ERROR);
		return;
	}
	leadingView.resetReading();
	linkView.resetReading();

	OGRFeature * lead_feat = 0;
	linkView.createSpatialIndex();
	while (lead_feat = leadingView.getNextFeature()) {
		OGRGeometry * lead_geo = lead_feat->GetGeometryRef();

		if (!lead_geo) {
			DM::Logger(DM::Warning) << "Lead feature geometry is null";
			continue;
		}

		OGRFeature * f = linkView.findNearestPoint((OGRPoint*) lead_geo, max_distance);
		if (!f)
			continue;
		lead_feat->SetField(link_name.c_str(), (int) f->GetFID());
	}
}
