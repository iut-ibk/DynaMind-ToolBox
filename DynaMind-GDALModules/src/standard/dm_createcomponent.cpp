#include "dm_createcomponent.h"
#include <ogrsf_frmts.h>

DM_DECLARE_CUSTOM_NODE_NAME(DM_CreateComponent, Create Single Component, Data Handling)

DM_CreateComponent::DM_CreateComponent()
{
	this->GDALModule = true;

	this->addParameter("lead_view_name", DM::STRING, &this->lead_view_name);
	this->lead_view_name = "city";

	//dummy to get the ports
	std::vector<DM::ViewContainer> data;
	data.push_back(  DM::ViewContainer ("dummy", DM::SUBSYSTEM, DM::WRITE) );
	this->addGDALData("city", data);
}

void DM_CreateComponent::init()
{
	std::vector<DM::ViewContainer*> data;

	if (!this->lead_view_name.empty())	{
		lead_view = DM::ViewContainer(this->lead_view_name, DM::FACE, DM::WRITE);
		data.push_back(&lead_view);
	}

	this->registerViewContainers(data);
}

void DM_CreateComponent::run()
{
	OGRFeature * f =lead_view.createFeature();


}

