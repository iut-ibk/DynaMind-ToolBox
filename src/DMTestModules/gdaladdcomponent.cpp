#include "gdaladdcomponent.h"
#include "ogrsf_frmts.h"
#include "dmgdalsystem.h"

DM_DECLARE_NODE_NAME(GDALAddComponent, Modules)

GDALAddComponent::GDALAddComponent()
{
	GDALModule = true;
	elements = 100000;
	this->addParameter("elements", DM::INT, &elements);
	append = false;
	this->addParameter("append", DM::BOOL, &append);
}

void GDALAddComponent::init()
{
	if (!append) {
		components = DM::ViewContainer("component", DM::NODE, DM::WRITE);
	}
	else {
		components = DM::ViewContainer("component", DM::NODE, DM::MODIFY);
	}
	components.addAttribute("persons",DM::Attribute::STRING, DM::WRITE);

	std::vector<DM::View> datastream;
	datastream.push_back(components);
	this->addData("city", datastream);
}


void GDALAddComponent::run()
{
	DM::GDALSystem * sys = this->getGDALData("city");
	sys->updateView(components);

	this->components.setCurrentGDALSystem(sys);

	int counter = 0;
	for (int i = 0; i < elements; i++) {
		OGRFeature * f = this->components.createFeature();
		f->SetField("persons", "that is me");
		OGRPoint pt;
		pt.setX( i );
		pt.setY( i );

		f->SetGeometry(&pt);
		if (counter == 100000) {
			components.syncAlteredFeatures();
			counter = 0;
		}
		counter++;
	}
	this->components.syncAlteredFeatures();
}
