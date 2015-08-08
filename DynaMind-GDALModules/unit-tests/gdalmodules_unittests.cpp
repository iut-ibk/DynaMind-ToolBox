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
//#define IMPORTWITHGDAL
//#define PARCELWIREDSHAPE
//#define FILTERINSPATIALLINKING
#define TESTMULTITHREADING

#ifdef IMPORTWITHGDAL
/**
 * @brief Test self intersection warning in GDAL
 */
TEST_F(GDALModules_Unittests, GDALSelfIntersect) {
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
	DM::Logger(DM::Standard) << "Create System";

	DM::Simulation * sim = new DM::Simulation();
	QDir dir("./");
	DM::SimulationConfig simconf;
	simconf.setCoordinateSystem(4283);
	sim->setSimulationConfig(simconf);
	sim->registerModulesFromDirectory(dir);
	DM::Module * rect = sim->addModule("GDALImportData");
	rect->setParameterValue("source", "../test_data/self_itersect.shp");
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
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
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

	DM::Module * export_sim_db = sim->addModule("DM_ExportSimulationDB");

	QString filename = QDir::tempPath() + "/" + QUuid::createUuid().toString();


	export_sim_db->setParameterValue("file_name", filename.toStdString());


	sim->addLink(attr_calc, "city",export_sim_db, "city");


	attr_calc->init();

	sim->run();

	//Get database


	delete sim;

	DM::Logger(DM::Standard) << "Done part 1";

	sim = new DM::Simulation();
	sim->registerModulesFromDirectory(dir);
	DM::Module * hotstart = sim->addModule("GDALHotStarter");

	hotstart->setParameterValue("hot_start_database", filename.toStdString() + ".sqlite");
	hotstart->init();

	DM::Module * attr_calc_counter = sim->addModule("GDALAttributeCalculator");

	attr_calc_counter->setParameterValue("attribute", "cityblock.sum");
	attr_calc_counter->setParameterValue("equation", "a");
	attr_calc_counter->setParameterValue("variables", "*||*a*|*cityblock.parcel.counter");

	sim->addLink(hotstart, "city",attr_calc_counter, "city");
	attr_calc_counter->init();

	sim->run();

	//Check M2
	DM::GDALSystem * sys = (DM::GDALSystem*) attr_calc_counter->getOutPortData("city");
	DM::ViewContainer components("cityblock", DM::FACE, DM::READ);
	components.setCurrentGDALSystem(sys);
	OGRFeature * f;
	components.resetReading();
	while (f = components.getNextFeature() ) {
		ASSERT_EQ(f->GetFieldAsDouble("sum"), 64);
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

// unit test to address issue #300
// https://github.com/iut-ibk/DynaMind-ToolBox/issues/300
#ifdef PARCELWIREDSHAPE
TEST_F(GDALModules_Unittests, ParcelWiredShape) {
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);

	DM::Simulation sim;
	DM::SimulationConfig sim_cfg = sim.getSimulationConfig();
	sim_cfg.setCoordinateSystem(32755);
	sim.setSimulationConfig(sim_cfg);
	QDir dir("./");
	sim.registerModulesFromDirectory(dir);
	DM::Module * imp = sim.addModule("GDALImportData");
	imp->setParameterValue("source", "../test_data/complex_polygon_1.geojson");
	imp->setParameterValue("layer_name", "OGRGeoJSON");
	imp->setParameterValue("view_name", "cityblock");
	imp->setParameterValue("epsg_from", "32755");
	imp->init();

	DM::Module * parceling = sim.addModule("GDALParceling");
	parceling->setParameterValue("blockName", "cityblock");
	parceling->setParameterValue("width", "200");
	parceling->setParameterValue("height", "70");

	sim.addLink(imp, "city",parceling, "city");

	sim.run();
}
#endif

// test_filter_in_spatial_linking
#ifdef FILTERINSPATIALLINKING
TEST_F(GDALModules_Unittests, SetFilterInSpatialLinking) {
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);

	DM::Simulation sim;
	DM::SimulationConfig sim_cfg = sim.getSimulationConfig();
	sim_cfg.setCoordinateSystem(32755);
	sim.setSimulationConfig(sim_cfg);
	QDir dir("./");
	sim.registerModulesFromDirectory(dir);
	DM::Module * imp = sim.addModule("GDALImportData");
	imp->setParameterValue("source", "../test_data/sa1_part.geojson");
	imp->setParameterValue("layer_name", "OGRGeoJSON");
	imp->setParameterValue("view_name", "sa1");
	imp->setParameterValue("epsg_from", "4283");
	imp->init();

	DM::Module * nodes = sim.addModule("GDALImportData");
	nodes->setParameterValue("source", "../test_data/sa1_part_nodes.geojson");
	nodes->setParameterValue("layer_name", "OGRGeoJSON");
	nodes->setParameterValue("view_name", "sa1_nodes");
	nodes->setParameterValue("append", "1");
	nodes->setParameterValue("epsg_from", "4283");
	nodes->init();

	sim.addLink(imp, "city",nodes, "city");

	DM::Module * nodes1 = sim.addModule("GDALImportData");
	nodes1->setParameterValue("source", "../test_data/sa1_part_nodes.geojson");
	nodes1->setParameterValue("layer_name", "OGRGeoJSON");
	nodes1->setParameterValue("view_name", "sa1_nodes_filter");
	nodes1->setParameterValue("append", "1");
	nodes1->setParameterValue("epsg_from", "4283");
	nodes1->init();

	sim.addLink(nodes, "city",nodes1, "city");

	DM::Module * nodes2 = sim.addModule("GDALImportData");
	nodes2->setParameterValue("source", "../test_data/sa1_part_nodes.geojson");
	nodes2->setParameterValue("layer_name", "OGRGeoJSON");
	nodes2->setParameterValue("view_name", "sa1_nodes_filter_2");
	nodes2->setParameterValue("append", "1");
	nodes2->setParameterValue("epsg_from", "4283");
	nodes2->init();

	sim.addLink(nodes1, "city",nodes2, "city");

	DM::Module * no_filter = sim.addModule("GDALSpatialLinking");
	no_filter->setParameterValue("leadingViewName", "sa1");
	no_filter->setParameterValue("linkViewName", "sa1_nodes");
	no_filter->setParameterValue("experimental", "1");

	sim.addLink(nodes2, "city",no_filter, "city");

	DM::Module * filter_2 = sim.addModule("GDALSpatialLinking");
	filter_2->setParameterValue("leadingViewName", "sa1");
	filter_2->setParameterValue("linkViewName", "sa1_nodes_filter_2");
	filter_2->setParameterValue("experimental", "1");

	sim.addLink(no_filter, "city",filter_2, "city");

	DM::Module * with_filter = sim.addModule("GDALSpatialLinking");
	with_filter->setParameterValue("leadingViewName", "sa1");
	with_filter->setParameterValue("linkViewName", "sa1_nodes_filter");
	with_filter->setParameterValue("experimental", "1");

	std::vector<DM::Filter> filters;
	DM::Filter attribute("sa1_nodes_filter", DM::FilterArgument("region_id = 2112218"));
	filters.push_back(attribute);
	with_filter->setFilter(filters);
	sim.addLink(filter_2, "city",with_filter, "city");

	DM::Module * with_filter_2 = sim.addModule("GDALSpatialLinking");
	with_filter_2->setParameterValue("leadingViewName", "sa1");
	with_filter_2->setParameterValue("linkViewName", "sa1_nodes_filter_2");
	with_filter_2->setParameterValue("experimental", "1");


	DM::Filter attribute_2("sa1", DM::FilterArgument("median_total_household_income_weekly = 1006"));
	filters.clear();
	filters.push_back(attribute_2);
	with_filter_2->setFilter(filters);
	sim.addLink(with_filter, "city",with_filter_2, "city");

	sim.run();

	DM::GDALSystem * sys = (DM::GDALSystem*) with_filter_2->getOutPortData("city");
	DM::ViewContainer components("sa1_nodes", DM::NODE, DM::READ);
	components.setCurrentGDALSystem(sys);
	OGRFeature * f;
	components.resetReading();
	int linked_elements_no_filter = 0;
	while (f = components.getNextFeature() ) {
		if ( f->GetFieldAsInteger("sa1_id") > 0)
			linked_elements_no_filter++;
	}

	int linked_elements_filter = 0;
	DM::ViewContainer components_f("sa1_nodes_filter", DM::NODE, DM::READ);
	components_f.setCurrentGDALSystem(sys);
	components_f.resetReading();
	while (f = components_f.getNextFeature() ) {
		if ( f->GetFieldAsInteger("sa1_id") > 0)
			linked_elements_filter++;
	}

	int linked_elements_filter_2 = 0;
	DM::ViewContainer components_f_2("sa1_nodes_filter_2", DM::NODE, DM::READ);
	components_f_2.setCurrentGDALSystem(sys);
	components_f_2.resetReading();
	while (f = components_f_2.getNextFeature() ) {
		if ( f->GetFieldAsInteger("sa1_id") > 0)
			linked_elements_filter_2++;
	}
	DM::Logger(DM::Error) << linked_elements_no_filter;
	DM::Logger(DM::Error) << linked_elements_filter;
	DM::Logger(DM::Error) << linked_elements_filter_2;
}
#endif

//Currently faiels because the cooridnates are currently crap
#ifdef TESTMULTITHREADING
TEST_F(GDALModules_Unittests, Multithrading) {
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Debug);
	DM::Logger(DM::Standard) << "Create System";

	DM::Simulation sim;
	QDir dir("./");
	sim.registerModulesFromDirectory(dir);
	DM::Module * rect = sim.addModule("GDALCreateRectangle");
	rect->setParameterValue("width", "10000");
	rect->setParameterValue("height", "10000");
	rect->setParameterValue("view_name", "superblock");
	rect->init();

	//Create City Blocks
	DM::Module * cityblock_tmp = sim.addModule("GDALParceling");
	cityblock_tmp->setParameterValue("blockName", "superblock");
	cityblock_tmp->setParameterValue("subdevisionName", "cityblock_tmp");
	cityblock_tmp->setParameterValue("width", "1000");
	cityblock_tmp->setParameterValue("height", "1000");

	sim.addLink(rect, "city",cityblock_tmp, "city");

	//Create City Blocks
	DM::Module * cityblock = sim.addModule("GDALOffset");
	cityblock->setParameterValue("blockName", "cityblock_tmp");
	cityblock->setParameterValue("subdevisionName", "cityblock");
	cityblock->setParameterValue("offset", "10");

	sim.addLink(cityblock_tmp, "city",cityblock, "city");

	//Create City Blocks
	DM::Module * parcel_tmp = sim.addModule("GDALParceling");
	parcel_tmp->setParameterValue("blockName", "cityblock_tmp");
	parcel_tmp->setParameterValue("subdevisionName", "parcel_tmp");
	parcel_tmp->setParameterValue("width", "100");
	parcel_tmp->setParameterValue("height", "100");

	sim.addLink(cityblock, "city",parcel_tmp, "city");

	//Create City Blocks
	DM::Module * parcel = sim.addModule("GDALParcelSplit");
	parcel->setParameterValue("blockName", "parcel_tmp");
	parcel->setParameterValue("subdevisionName", "parcel");
	parcel->setParameterValue("width", "15");


	sim.addLink(parcel_tmp, "city",parcel, "city");
	parcel->init();
	//Create City Blocks
	DM::Module * parcel_area = sim.addModule("GDALGeometricAttributes");
	parcel_area->setParameterValue("leading_view", "parcel");
	parcel_area->setParameterValue("calculate_area", "1");
	parcel_area->setParameterValue("aspect_ratio_BB", "1");
	parcel_area->setParameterValue("percentage_filled", "1");
	parcel_area->init();
	sim.addLink(parcel, "city",parcel_area, "city");

	for (int i = 0; i < 100 ; i++) {
		sim.reset();
		sim.run();
		//Check M2
		DM::GDALSystem * sys = (DM::GDALSystem*) parcel_area->getOutPortData("city");
		DM::ViewContainer components = DM::ViewContainer("parcel", DM::FACE, DM::READ);
		components.setCurrentGDALSystem(sys);
		ASSERT_EQ(components.getFeatureCount(), 81920);
	}
}
#endif

