#include "gdallinkedcomponent.h"
#include "ogrsf_frmts.h"


DM_DECLARE_NODE_NAME(GDALLinkedComponent, Modules)

GDALLinkedComponent::GDALLinkedComponent()
{
	GDALModule = true;
	elements = 100000;
	this->addParameter("elements", DM::INT, &elements);
	append = false;
	this->addParameter("append", DM::BOOL, &append);
}


void GDALLinkedComponent::init()
{

	components = DM::ViewContainer("component", DM::COMPONENT, DM::READ);
	components.addAttribute("control_value", DM::Attribute::INT, DM::WRITE);
	linked_components = DM::ViewContainer("linked", DM::COMPONENT, DM::WRITE);
	linked_components.addAttribute("component_id", "component", DM::WRITE);
	linked_components.addAttribute("control_value", DM::Attribute::INT, DM::WRITE);

	std::vector<DM::ViewContainer * > datastream;
	datastream.push_back(&components);
	datastream.push_back(&linked_components);
	this->registerViewContainers(datastream);
}


void GDALLinkedComponent::run()
{
	int counter = 0;
	OGRFeature * cmp;
	components.resetReading();
	while (cmp = components.getNextFeature()) {
		int control_value = rand() % 100000;
		OGRFeature * f = this->linked_components.createFeature();
		f->SetField("control_value", control_value);
		cmp->SetField("control_value", control_value);
		linked_components.linkFeatures(f, cmp);
		counter++;
	}
}

