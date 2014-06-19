#include "updateallcomponents.h"
#include "ogrsf_frmts.h"
#include "dmgdalsystem.h"
#include "QTimer"


DM_DECLARE_NODE_NAME(UpdateAllComponents, Modules)

UpdateAllComponents::UpdateAllComponents()
{
	GDALModule = true;
	elements = 100000;
	this->addParameter("elements", DM::INT, &elements);

}

void UpdateAllComponents::init()
{
	components = DM::ViewContainer("component", DM::NODE, DM::READ);
	components.addAttribute("test", DM::Attribute::DOUBLE, DM::WRITE);

	std::vector<DM::View> datastream;
	datastream.push_back(components);
	this->addData("city", datastream);
}

void UpdateAllComponents::run()
{
	QTime myTimer;
	myTimer.start();

	components.setCurrentGDALSystem(this->getGDALData("city"));

	DM::Logger(DM::Standard) << "Prepare "<<  myTimer.elapsed();

	myTimer.restart();

	int sum = 0;
	OGRFeature * f;
	while ( (f = components.getNextFeature()) != NULL ) {
		OGRFeatureDefn * fdef =  f->GetDefnRef();
		int index = fdef->GetFieldIndex("test");
		f->SetField("test", 2.);
		sum+=f->GetFID();
	}
	components.syncAlteredFeatures();
	DM::Logger(DM::Standard) << "Read "<<  myTimer.elapsed();
	DM::Logger(DM::Standard) << "Sum" << sum;

}

