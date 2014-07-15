#include "lsystems.h"

#include "dmgdalsystem.h"


#include <ogr_api.h>

DM_DECLARE_NODE_NAME(LSystems, GDALModules)

LSystems::LSystems()
{
	GDALModule = true;

	view = DM::ViewContainer("Lsystem", DM::EDGE, DM::WRITE);

	std::vector<DM::ViewContainer> datastream;
	datastream.push_back(view);

	this->addGDALData("city", datastream);
}

void LSystems::run() {
	DM::GDALSystem * sys = this->getGDALData("city");
	view.setCurrentGDALSystem(sys);




}


Extend::Extend(OGRPoint pt)
{
	this->start = pt;
}

void Extend::run()
{

}
