#include "gdalmodules_unittests.h"
#include <dmsimulation.h>
#include <dmlogger.h>
#include <dmlog.h>
#include <dmlogsink.h>
#include <dmviewcontainer.h>
#include <dmsystem.h>
#include <dmmodule.h>
#include <dmgdalsystem.h>
#include <dmviewcontainer.h>
#include <ogrsf_frmts.h>
#include <dmsystem.h>

//#define GDALPARCELSPLIT
//#define GDALATTRIBUTECALCULATOR
//#define GDALHOTSTARTSIMULATION
#define IMPORTWITHGDAL


#ifdef IMPORTWITHGDAL
/**
 * @brief Test self intersection warning in GDAL
 */
TEST_F(GDALModules_Unittests, GDALSelfIntersect) {
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);
	DM::Logger(DM::Standard) << "Create System";

	DM::Simulation * sim = new DM::Simulation();
	QDir dir("./");
	DM::SimulationConfig simconf;
	simconf.setCoordinateSystem(4283);
	sim->setSimulationConfig(simconf);
	sim->registerModulesFromDirectory(dir);
	DM::Module * rect = sim->addModule("GDALImportData");
	rect->setParameterValue("source", "/Users/christianurich/Documents/DynaMind-ToolBox/DynaMind-GDALModules/test_data/self_itersect.shp");
	rect->setParameterValue("layer_name", "self_itersect");
	rect->setParameterValue("view_name", "test");
	rect->setParameterValue("epsg_from", "32755");
	rect->setParameterValue("epsg_to", "4283");
	sim->run();

}
#endif

#ifdef GDALHOTSTARTSIMULATION

/**
 * @brief Test the GDALHotStartSimulation. The first simulation is just used to create
 * an initial dataset. The second part tests the hotstart
 */
TEST_F(GDALModules_Unittests, GDALHostStartSimulation) {
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);
	DM::Logger(DM::Standard) << "Create System";

	DM::Simulation * sim = new DM::Simulation();
	QDir dir("./");
	sim->registerModulesFromDirectory(dir);
	DM::Module * rect = sim->addModule("GDALCreateRectangle");
	rect->setParameterValue("width", "800");
	rect->setParameterValue("height", "800");
	rect->setParameterValue("view_name", "cityblock");
	rect->init();


	DM::Module * parceling = sim->addModule("GDALParceling");

	parceling->setParameterValue("width", "100");
	parceling->setParameterValue("height", "100");

	sim->addLink(rect, "city",parceling, "city");


	DM::Module * linking = sim->addModule("GDALSpatialLinking");

	linking->setParameterValue("leadingViewName", "cityblock");
	linking->setParameterValue("linkViewName", "parcel");

	sim->addLink(parceling, "city",linking, "city");
	linking->init();

	DM::Module * attr_calc = sim->addModule("GDALAttributeCalculator");

	attr_calc->setParameterValue("attribute", "parcel.counter");
	attr_calc->setParameterValue("equation", "1");

	sim->addLink(linking, "city",attr_calc, "city");
	attr_calc->init();

	sim->run();

	//Get database

	DM::GDALSystem * sys = (DM::GDALSystem*) attr_calc->getOutPortData("city");
	std::string db_name = sys->getDBID();

	delete sim;

	DM::Logger(DM::Standard) << "Done part 1";

	sim = new DM::Simulation();
	sim->registerModulesFromDirectory(dir);
	DM::Module * hotstart = sim->addModule("GDALHotStarter");
	hotstart->setParameterValue("hot_start_database", "/tmp/" + db_name + ".db");
	hotstart->init();

	DM::Module * attr_calc_counter = sim->addModule("GDALAttributeCalculator");

	attr_calc_counter->setParameterValue("attribute", "cityblock.sum");
	attr_calc_counter->setParameterValue("equation", "a");
	attr_calc_counter->setParameterValue("variables", "*||*a*|*cityblock.parcel.counter");

	sim->addLink(hotstart, "city",attr_calc_counter, "city");
	attr_calc_counter->init();

	sim->run();

	//Check M2
	sys = (DM::GDALSystem*) attr_calc_counter->getOutPortData("city");
	DM::ViewContainer components("cityblock", DM::FACE, DM::READ);
	components.setCurrentGDALSystem(sys);
	OGRFeature * f;
	components.resetReading();
	while (f = components.getNextFeature() ) {
		ASSERT_EQ(f->GetFieldAsDouble("sum"), 64);
		delete sim;
		return;
	}
	delete sim;
	ASSERT_TRUE(false);


}
#endif

//Currently faiels because the cooridnates are currently crap
#ifdef GDALPARCELSPLIT
TEST_F(GDALModules_Unittests, GDALParcelSplit) {
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
	DM::Logger(DM::Standard) << "Create System";

	DM::Simulation sim;
	QDir dir("./");
	sim.registerModulesFromDirectory(dir);
	DM::Module * rect = sim.addModule("GDALCreateRectangle");
	rect->setParameterValue("width", "100");
	rect->setParameterValue("height", "200");
	rect->setParameterValue("view_name", "cityblock");
	rect->init();


	DM::Module * parceling = sim.addModule("GDALParcelSplit");
	sim.addLink(rect, "city",parceling, "city");


	sim.run();

	//Check M2
	DM::GDALSystem * sys = (DM::GDALSystem*) parceling->getOutPortData("city");
	DM::ViewContainer components = DM::ViewContainer("parcel", DM::FACE, DM::READ);
	components.setCurrentGDALSystem(sys);
	ASSERT_EQ(components.getFeatureCount(), 26);

	sim.clear();

	rect = sim.addModule("GDALCreateRectangle");
	rect->setParameterValue("width", "200");
	rect->setParameterValue("height", "100");
	rect->setParameterValue("view_name", "cityblock");
	rect->init();


	 parceling = sim.addModule("GDALParcelSplit");
	sim.addLink(rect, "city",parceling, "city");


	sim.run();

	//Check M2
	sys = (DM::GDALSystem*) parceling->getOutPortData("city");
	components = DM::ViewContainer("parcel", DM::FACE, DM::READ);
	components.setCurrentGDALSystem(sys);
	ASSERT_EQ(components.getFeatureCount(), 26);

}
#endif

#ifdef GDALATTRIBUTECALCULATOR
TEST_F(GDALModules_Unittests, GDALAttributeCaluclator) {
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
	DM::Logger(DM::Standard) << "Create System";

	DM::Simulation sim;
	QDir dir("./");
	sim.registerModulesFromDirectory(dir);
	DM::Module * rect = sim.addModule("GDALCreateRectangle");
	rect->setParameterValue("width", "800");
	rect->setParameterValue("height", "800");
	rect->setParameterValue("view_name", "cityblock");
	rect->init();


	DM::Module * parceling = sim.addModule("GDALParceling");

	parceling->setParameterValue("width", "100");
	parceling->setParameterValue("height", "100");

	sim.addLink(rect, "city",parceling, "city");


	DM::Module * linking = sim.addModule("GDALSpatialLinking");

	linking->setParameterValue("leadingViewName", "cityblock");
	linking->setParameterValue("linkViewName", "parcel");

	sim.addLink(parceling, "city",linking, "city");
	linking->init();

	DM::Module * attr_calc = sim.addModule("GDALAttributeCalculator");

	attr_calc->setParameterValue("attribute", "parcel.counter");
	attr_calc->setParameterValue("equation", "1");

	sim.addLink(linking, "city",attr_calc, "city");
	attr_calc->init();


	DM::Module * attr_calc_counter = sim.addModule("GDALAttributeCalculator");

	attr_calc_counter->setParameterValue("attribute", "cityblock.sum");
	attr_calc_counter->setParameterValue("equation", "a");
	attr_calc_counter->setParameterValue("variables", "*||*a*|*cityblock.parcel.counter");

	sim.addLink(attr_calc, "city",attr_calc_counter, "city");
	attr_calc_counter->init();

	sim.run();
}
#endif


