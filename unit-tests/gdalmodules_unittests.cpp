#include "gdalmodules_unittests.h"
#include <dmsimulation.h>
#include <dmlogger.h>
#include <dmlog.h>
#include <dmlogsink.h>


TEST_F(GDALModules_Unittests, GDALParceling) {

}

TEST_F(GDALModules_Unittests, GDALAttributeCaluclator) {
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);
	DM::Logger(DM::Standard) << "Create System";

	DM::Simulation sim;
	QDir dir("./");
	sim.registerModulesFromDirectory(dir);
	DM::Module * rect = sim.addModule("GDALCreateRectangle");
	rect->setParameterValue("width", "800");
	rect->setParameterValue("height", "800");
	rect->setParameterValue("view_name", "CITYBLOCK");
	rect->init();


	DM::Module * parceling = sim.addModule("GDALParceling");

	parceling->setParameterValue("width", "100");
	parceling->setParameterValue("height", "100");

	sim.addLink(rect, "city",parceling, "city");


	DM::Module * linking = sim.addModule("GDALSpatialLinking");

	linking->setParameterValue("leadingViewName", "CITYBLOCK");
	linking->setParameterValue("linkViewName", "PARCEL");

	sim.addLink(parceling, "city",linking, "city");
	linking->init();

	DM::Module * attr_calc = sim.addModule("GDALAttributeCalculator");

	attr_calc->setParameterValue("attribute", "PARCEL.counter");
	attr_calc->setParameterValue("equation", "1");

	sim.addLink(linking, "city",attr_calc, "city");
	attr_calc->init();


	DM::Module * attr_calc_counter = sim.addModule("GDALAttributeCalculator");

	attr_calc_counter->setParameterValue("attribute", "CITYBLOCK.sum");
	attr_calc_counter->setParameterValue("equation", "a");
	attr_calc_counter->setParameterValue("variables", "*||*a*|*CITYBLOCK.PARCEL.counter");

	sim.addLink(attr_calc, "city",attr_calc_counter, "city");
	attr_calc_counter->init();

	sim.run();
}


