#include "gdalvalidatelinks.h"

#include "ogrsf_frmts.h"


DM_DECLARE_NODE_NAME(GDALValidateLinks, Modules)

GDALValidateLinks::GDALValidateLinks()
{
	GDALModule = true;
	elements = 100000;
	this->addParameter("elements", DM::INT, &elements);
	append = false;
	this->addParameter("append", DM::BOOL, &append);
}


void GDALValidateLinks::init()
{

	components = DM::ViewContainer("component", DM::COMPONENT, DM::READ);
	components.addAttribute("control_value", DM::Attribute::INT, DM::READ);
	linked_components = DM::ViewContainer("linked", DM::COMPONENT, DM::WRITE);
	linked_components.addAttribute("component_id", "component", DM::READ);
	linked_components.addAttribute("control_value", DM::Attribute::INT, DM::READ);

	std::vector<DM::ViewContainer * > datastream;
	datastream.push_back(&components);
	datastream.push_back(&linked_components);
	this->registerViewContainers(datastream);
}


void GDALValidateLinks::run()
{
	OGRFeature * f;
	linked_components.resetReading();
	while (f = components.getNextFeature()) {
		std::vector<OGRFeature*> linked = linked_components.getLinkedFeatures(f);
		if (linked.size() != 1) {
			DM::Logger(DM::Error) << "Linking failed";
			this->setStatus(DM::MOD_EXECUTION_ERROR);
			return;
		}
		if (f->GetFieldAsDouble("control_value") != linked[0]->GetFieldAsDouble("control_value")){
			DM::Logger(DM::Error) << "Linking failed control values don't match";
			this->setStatus(DM::MOD_EXECUTION_ERROR);
			return;
		}
	}
}

