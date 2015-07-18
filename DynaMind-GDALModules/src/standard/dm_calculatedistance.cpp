#include "dm_calculatedistance.h"

#include <sstream>
#include <ogrsf_frmts.h>
#include <dmgdalsystem.h>

DM_DECLARE_CUSTOM_NODE_NAME(DM_CalculateDistance,Calculate Distance, Data Handling)

DM_CalculateDistance::DM_CalculateDistance()
{
	GDALModule = true;
	properInit = false;

	this->lead_view_name = "";
	this->addParameter("lead_view_name", DM::STRING, &lead_view_name);

	this->distance_view_name = "";
	this->addParameter("distance_view_name", DM::STRING, &distance_view_name);

	this->attribute_name = "";
	this->addParameter("attribute_name", DM::STRING, &attribute_name);

	//dummy to get the ports
	std::vector<DM::ViewContainer> data;
	data.push_back(  DM::ViewContainer ("dummy", DM::SUBSYSTEM, DM::MODIFY) );
	this->addGDALData("city", data);
}

void DM_CalculateDistance::init()
{
	properInit = false;
	if (this->lead_view_name.empty())
		return;
	if (this->distance_view_name.empty())
		return;
	if (this->attribute_name.empty())
		return;
	std::map<std::string, DM::View> inViews = getViewsInStream()["city"];
	if (inViews.find(lead_view_name) == inViews.end()) {
		DM::Logger(DM::Warning) << "view " << lead_view_name << " not found in in stream";
		return;
	}
	if (inViews.find(distance_view_name) == inViews.end()) {
		DM::Logger(DM::Warning) << "view " << distance_view_name << " not found in in stream";
		return;
	}

	properInit=true;
	this->lead_view = DM::ViewContainer(this->lead_view_name, DM::COMPONENT, DM::READ);
	this->lead_view.addAttribute("distance", DM::Attribute::DOUBLE, DM::WRITE);
	this->distance_view = DM::ViewContainer(this->distance_view_name, DM::COMPONENT, DM::READ);

	std::vector<DM::ViewContainer*> data_stream;

	data_stream.push_back(&lead_view);
	data_stream.push_back(&distance_view);



	this->registerViewContainers(data_stream);
}

void DM_CalculateDistance::run()
{
	if (!properInit) {
		DM::Logger(DM::Error) << "DM_CalculateDistance init failed";
		this->setStatus(DM::MOD_CHECK_ERROR);
		return;
	}
	DM::GDALSystem * sys = this->getGDALData("city");

	std::stringstream query;

	query << "UPDATE " << this->lead_view_name << " SET distance = (SELECT SUM(ST_DISTANCE(";
	query << this->lead_view_name << ".geometry, b.geometry)) from " << this->distance_view_name <<" as b)";

	DM::Logger(DM::Debug) << query.str();
	OGRLayer * l = sys->getDataSource()->ExecuteSQL(query.str().c_str(), 0, "SQLITE");

	sys->getDataSource()->ReleaseResultSet(l);
}

string DM_CalculateDistance::getHelpUrl()
{
	return "/DynaMind-GDALModules/dm_calculatedistance.html";
}

