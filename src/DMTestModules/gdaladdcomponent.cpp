#include "gdaladdcomponent.h"
#include "ogrsf_frmts.h"
#include "dmgdalsystem.h"

DM_DECLARE_NODE_NAME(GDALAddComponent, Modules)

GDALAddComponent::GDALAddComponent()
{
	households = DM::ViewContainer("household", DM::COMPONENT, DM::WRITE);
	households.addAttribute("persons",DM::Attribute::STRING, DM::WRITE);

	std::vector<DM::View> datastream;

	datastream.push_back(households);
	this->addData("city", datastream);
}

void GDALAddComponent::run()
{
	DM::GDALSystem * sys = this->getGDALData();
	sys->updateSystemView(households);

	this->households.setCurrentGDALSystem(sys);

	for (int i = 0; i < 100000; i++) {
		OGRFeature * f = this->households.createFeature();
		f->SetField("persons", "that is me");
	}
	this->households.syncFeatures();

	DM::Logger(DM::Standard) << "Start";
}
