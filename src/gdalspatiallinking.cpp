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

	this->withCentroid = true;
	this->addParameter("withCentroid", DM::BOOL, &withCentroid);

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
	l_name << leadingViewName << "_id";
	link_name = l_name.str();
	this->linkView.addAttribute(link_name, DM::Attribute::INT, DM::WRITE);

	std::vector<DM::ViewContainer *> data_stream;
	data_stream.push_back(&leadingView);
	data_stream.push_back(&linkView);

	this->registerViewContainers(data_stream);

	properInit = true;
}

bool GDALSpatialLinking::checkCentroid(OGRGeometry* geo, OGRGeometry * lead_geo)
{
	OGRPoint ct;
	if (int error = geo->Centroid(&ct) != OGRERR_NONE) {
		DM::Logger(DM::Warning) << "error calculationg ct " << error;
		return false;
	}
	return ct.Within(lead_geo);
}

bool GDALSpatialLinking::checkIntersection(OGRGeometry* geo, OGRGeometry * lead_geo)
{
	return (geo->Within(lead_geo) || geo->Intersects(lead_geo));
}

void GDALSpatialLinking::run()
{
	if (!properInit) {
		DM::Logger(DM::Error) << "GDALSpatialLinking init failed";
		this->setStatus(DM::MOD_CHECK_ERROR);
		return;
	}
	leadingView.resetReading();

	OGRFeature * lead_feat = 0;
	while (lead_feat = leadingView.getNextFeature()) {

		OGRFeature * link_feature = 0;
		OGRGeometry * lead_geo = lead_feat->GetGeometryRef();
		int id = lead_feat->GetFID();

		if (!lead_geo) {
			DM::Logger(DM::Warning) << "Lead feature geometry is null";
			continue;
		}

		linkView.resetReading();
		linkView.setSpatialFilter(lead_geo);
		long counter = 0;
		while (link_feature = linkView.getNextFeature()) {

			OGRGeometry * geo = link_feature->GetGeometryRef();
			if (!geo){
				DM::Logger(DM::Warning) << "feature link feature geometry is null";
				continue;
			}
			bool isLinked = false;
			if (withCentroid)
				isLinked = checkCentroid(geo, lead_geo);
			else
				isLinked = checkIntersection(geo, lead_geo);
			if (!isLinked)
				continue;
			link_feature->SetField(link_name.c_str(), id);
			counter++;
		}
		if (counter % 100000){
			linkView.syncAlteredFeatures();
			leadingView.syncReadFeatures();
		}
	}
}
