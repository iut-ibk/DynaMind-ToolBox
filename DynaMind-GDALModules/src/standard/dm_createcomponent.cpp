#include "dm_createcomponent.h"
#include <ogrsf_frmts.h>

DM_DECLARE_CUSTOM_NODE_NAME(DM_CreateComponent, Create Single Component, Data Handling)

DM_CreateComponent::DM_CreateComponent()
{
	this->GDALModule = true;

	this->addParameter("lead_view_name", DM::STRING, &this->lead_view_name);
	this->lead_view_name = "city";

	this->component_type = "FACE";
	this->addParameter("component_type", DM::STRING, &this->component_type);

	this->create_empty = false;
	this->addParameter("create_empty", DM::BOOL, &this->create_empty);

	comp_as_int["COMPONENT"] = DM::COMPONENT;
	comp_as_int["NODE"] = DM::NODE;
	comp_as_int["EDGE"] = DM::EDGE;
	comp_as_int["FACE"] = DM::FACE;

	//dummy to get the ports
	std::vector<DM::ViewContainer> data;
	data.push_back(  DM::ViewContainer ("dummy", DM::SUBSYSTEM, DM::WRITE) );
	this->addGDALData("city", data);
}

void DM_CreateComponent::init()
{
	std::vector<DM::ViewContainer*> data;

	int ct = DM::COMPONENT;
	if (comp_as_int.find(this->component_type) != comp_as_int.end()) {
		ct = comp_as_int[component_type];
	} else {
		DM::Logger(DM::Error) << "Component type not set porperly supported types are COMPONENT, NODE, EDGE, FACE";
		this->setStatus(DM::MOD_CHECK_ERROR);

	}
	if (!this->lead_view_name.empty())	{
		lead_view = DM::ViewContainer(this->lead_view_name, ct , DM::WRITE);
		data.push_back(&lead_view);
	}
	this->registerViewContainers(data);
}

string DM_CreateComponent::getHelpUrl()
{
	return "/DynaMind-GDALModules/dm_createcomponent.html";
}

void DM_CreateComponent::run()
{
	int ct = DM::COMPONENT;
	if (comp_as_int.find(this->component_type) != comp_as_int.end()) {
		ct = comp_as_int[component_type];
	} else {
		DM::Logger(DM::Error) << "Component type not set porperly supported types are COMPONENT, NODE, EDGE, FACE";
		this->setStatus(DM::MOD_EXECUTION_ERROR);
		return;

	}
	if (!this->create_empty)
		OGRFeature * f =lead_view.createFeature();

}

