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
#include <sqlite3.h>
#include <dmsystem.h>

#define GDALPARCELSPLIT
#define GDALATTRIBUTECALCULATOR
//#define GDALHOTSTARTSIMULATION
//#define IMPORTWITHGDAL
//#define PARCELWIREDSHAPE
//#define FILTERINSPATIALLINKING
#define TESTMULTITHREADING
//#define LOADEXTENSION

#define ELEMENTS 1000

#ifdef LOADEXTENSION
/**
 * @brief Test self intersection warning in GDAL
 */

int execute_sql_statement(sqlite3 *db, char *sql){
	/* Execute SQL statement */
	const char* data = "Callback function called";
	char *zErrMsg = 0;

	int rc = sqlite3_exec(db, sql, 0, (void*)data, &zErrMsg);
	if( rc != SQLITE_OK ){
		DM::Logger(DM::Error) <<" SQL statment: " << QString::fromLatin1(sql).toStdString();
		DM::Logger(DM::Error) <<" SQL error: " << QString::fromLatin1(zErrMsg).toStdString();
	   EXPECT_TRUE( rc == SQLITE_OK );
	   sqlite3_free(zErrMsg);
	}

	return rc;
}

TEST_F(GDALModules_Unittests, LoadSpatialiteExtension) {
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);

	// Create test container for DB to get DM connection
	DM::ViewContainer * TestComponents = new DM::ViewContainer("TestComponents", DM::COMPONENT, DM::WRITE);
	DM::GDALSystem sys;
	sys.updateView(*TestComponents);
	TestComponents->setCurrentGDALSystem(&sys);
	delete TestComponents;


	sqlite3 *db;
	int rc = sqlite3_open(sys.getDBID().c_str(), &db);
	sqlite3_enable_load_extension(db,1);

	if( rc ){
		EXPECT_TRUE( rc == SQLITE_OK );
		DM::Logger(DM::Error) << "Failed to open database";
		return;
	}

	//Load spatialite
	execute_sql_statement(db, "SELECT load_extension('mod_spatialite')");
	execute_sql_statement(db, "SELECT load_extension('libdm_sqlite_plugin')");

	execute_sql_statement(db, "select dm_poly_percentage_filled('POLYGON((336332.0601752918 5778721.705329683,336346.7535427466 5778768.016486913,336351.3961634005 5778782.745384061,336334.8932253966 5778784.403286382,336318.3902387228 5778786.063152879,336301.8872837827 5778787.720988676,336285.3842801625 5778789.380788642,336268.2326900881 5778791.104762079,336260.781057615 5778791.854045309,336253.6909351451 5778792.567083915,336263.698832185 5778829.479009006,336242.8915983241 5778831.512546347,336227.2844455204 5778833.039184218,336211.8097682864 5778834.549699385,336214.131482249 5778852.905378218,336214.156459031 5778853.102153304,336217.8797259022 5778882.528357361,336217.9038727357 5778882.718454879,336221.6288571875 5778912.156900431,336221.667486138 5778912.538010908,336222.4699038102 5778920.594662376,336223.3328787678 5778930.721937167,336225.0943103255 5778948.927667666,336226.7635319579 5778967.018306022,336228.8610000518 5778984.729796771,336228.8849722177 5778984.9765076,336229.5469227375 5778991.817837848,336244.43751157 5778990.296511691,336259.4237860427 5778988.765761775,336274.5850456461 5778987.215860683,336292.8048473224 5778985.352941507,336306.6881488096 5778983.933895792,336317.2205704625 5779021.796116182,336325.1746517969 5779021.016363115,336332.9779798029 5779020.249554403,336446.2490053769 5779009.130911333,336440.7515266768 5778991.803169123,336435.2195177577 5778974.370929433,336429.6876109395 5778956.934911831,336418.7182595087 5778922.358279723,336413.1287528263 5778904.816290314,336407.5211815296 5778887.214539694,336400.6959373071 5778865.791182512,336393.9350592956 5778844.574819867,336387.3530785431 5778823.914118354,336398.1149398593 5778822.805800474,336414.6052348465 5778821.107769582,336400.9059712268 5778777.767502066,336398.5771263694 5778770.439719696,336396.3181305562 5778763.092582113,336387.6884681875 5778763.949385753,336380.266052903 5778764.687041802,336376.4317121722 5778752.645855415,336372.9339455281 5778741.664401952,336365.1291334924 5778717.159312944,336348.7020442281 5778719.417014452,336341.7576164636 5778720.371848284,336332.0601752918 5778721.705329683))')");

	sqlite3_close(db);

	return;

}
#endif




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
	parcel_area->setParameterValue("useSQL", "1");
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

