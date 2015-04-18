#include "testgdalmodules.h"

#include <dmsimulation.h>
#include <dmlogger.h>
#include <dmlog.h>
#include <dmlogsink.h>
#include <ogrsf_frmts.h>
#include <dmviewcontainer.h>
#include <dmsystem.h>
#include <dmgdalhelper.h>
#include <QDir>

#define FEATURES "1000"
#define FEATURES_2 "2000"
#define DEFAULTEPSG 31254

#define SPEEDTESTDM
#define SPEEDTEST
#define CONNECTIONTEST
#define BRANCHINGTEST
#define EXPANDING
#define BRANCHMODIFY
#define GDALVCAPITEST
#define WRITEATTRIBUTES
#define TESTLINKS
#define TESTSYSTEMSETTINGS
#define TESTSAVELOAD

bool clean_workingdir(QString dirname) {
	bool result = true;
	QDir dir(dirname);

	if (dir.exists(dirname)) {
		Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
			if (info.isDir()) {
				result = clean_workingdir(info.absoluteFilePath());
			}
			else {
				result = QFile::remove(info.absoluteFilePath());
			}

			if (!result) {
				return result;
			}
		}
		result = dir.rmdir(dirname);
	}
	return result;
}


DM::Simulation * startup(QString working_dir = ""){
	if (working_dir.isEmpty()) {
		working_dir = QDir::tempPath()+"/dynamind";
	}
	clean_workingdir(working_dir);
	return new DM::Simulation();
}

bool cleanup(DM::Simulation * sim, QString working_dir = "") {
	delete sim;
	if (working_dir.isEmpty()) {
		working_dir = QDir::tempPath()+"/dynamind";
	}
	QDir dir(working_dir);
	if (dir.exists(working_dir)) {
		Q_FOREACH(QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
			if (!info.isDir()) {
				return false; // directory not empty;
			}
		}
	}

	return true;
}

#ifdef GDALVCAPITEST
TEST_F(TestGDALModules,GDAL_ADVANCE_API_TEST) {
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
	DM::Logger(DM::Standard) << "Create System";

	DM::Simulation * sim = startup();
	DM::SimulationConfig conf;
	conf.setCoordinateSystem(DEFAULTEPSG);
	sim->setSimulationConfig(conf);
	QDir dir("./");
	sim->registerModulesFromDirectory(dir);
	DM::Module * m1 = sim->addModule("GDALAddComponentViewContainer");
	m1->setParameterValue("elements", FEATURES);

	DM::Module * m2 = sim->addModule("GDALAddComponentViewContainer");
	m2->setParameterValue("elements", FEATURES);
	m2->setParameterValue("append", "1");
	m2->init();
	sim->addLink(m1, "city", m2, "city");
	sim->run();

	DM::GDALSystem * sys = (DM::GDALSystem*) m2->getOutPortData("city");
	DM::ViewContainer components = DM::ViewContainer("component", DM::NODE, DM::READ);
	components.setCurrentGDALSystem(sys);

	QString s_number = QString(FEATURES);
	int number = 2* s_number.toInt();
	ASSERT_EQ(components.getFeatureCount(), number);

	ASSERT_TRUE(cleanup(sim));

}
#endif

#ifdef SPEEDTESTDM
TEST_F(TestGDALModules,TestInsertSpeed_DM) {

	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
	DM::Logger(DM::Standard) << "Create System";

	DM::Simulation * sim = startup();
	DM::SimulationConfig conf;
	conf.setCoordinateSystem(DEFAULTEPSG);
	sim->setSimulationConfig(conf);
	QDir dir("./");
	sim->registerModulesFromDirectory(dir);
	sim->addModule("GDALAddComponent")->setParameterValue("elements", FEATURES);
	sim->run();

	ASSERT_TRUE(cleanup(sim));
}
#endif

#ifdef SPEEDTEST
TEST_F(TestGDALModules,TestInsertSpeed) {

	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
	DM::Logger(DM::Standard) << "Create System";

	DM::Simulation * sim = startup();
	DM::SimulationConfig conf;
	conf.setCoordinateSystem(DEFAULTEPSG);
	sim->setSimulationConfig(conf);
	QDir dir("./");
	sim->registerModulesFromDirectory(dir);
	sim->addModule("GDALAddComponent")->setParameterValue("elements", FEATURES);
	sim->run();
	ASSERT_TRUE(cleanup(sim));
}
#endif

#ifdef CONNECTIONTEST
TEST_F(TestGDALModules,ConnectionTest) {

	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
	DM::Logger(DM::Standard) << "Create System";

	DM::Simulation * sim = startup();
	DM::SimulationConfig conf;
	conf.setCoordinateSystem(DEFAULTEPSG);
	sim->setSimulationConfig(conf);
	QDir dir("./");
	sim->registerModulesFromDirectory(dir);
	DM::Module * m1 = sim->addModule("GDALAddComponent");
	m1->setParameterValue("elements", FEATURES);

	DM::Module * m2 = sim->addModule("GDALAddComponent");
	m2->setParameterValue("elements", FEATURES);
	m2->setParameterValue("append", "1");
	m2->init();
	sim->addLink(m1, "city", m2, "city");
	sim->run();

	DM::GDALSystem * sys = (DM::GDALSystem*) m2->getOutPortData("city");
	DM::ViewContainer components = DM::ViewContainer("component", DM::NODE, DM::READ);
	components.setCurrentGDALSystem(sys);

	QString s_number = QString(FEATURES);
	int number = 2* s_number.toInt();
	ASSERT_EQ(components.getFeatureCount(), number);

	ASSERT_TRUE(cleanup(sim));
}
#endif

#ifdef BRANCHINGTEST
TEST_F(TestGDALModules,BranchingTest) {

	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Warning);
	DM::Logger(DM::Standard) << "Create System";

	DM::Simulation * sim = startup();
	DM::SimulationConfig conf;
	conf.setCoordinateSystem(DEFAULTEPSG);
	sim->setSimulationConfig(conf);
	QDir dir("./");
	sim->registerModulesFromDirectory(dir);

	DM::Module * m1 = sim->addModule("GDALAddComponent");
	m1->setParameterValue("elements", FEATURES);

	DM::Module * m2 = sim->addModule("GDALAddComponent");
	m2->setParameterValue("elements", FEATURES);
	m2->setParameterValue("append", "1");
	m2->init();
	sim->addLink(m1, "city", m2, "city");

	DM::Module * m3 = sim->addModule("GDALAddComponent");
	m3->setParameterValue("elements", FEATURES_2);
	m3->setParameterValue("append", "1");
	m3->init();
	sim->addLink(m1, "city", m3, "city");

	sim->run();

	//Check M2
	DM::GDALSystem * sys = (DM::GDALSystem*) m2->getOutPortData("city");
	DM::ViewContainer components = DM::ViewContainer("component", DM::NODE, DM::READ);
	components.setCurrentGDALSystem(sys);

	QString s_number = QString(FEATURES);
	int number = 2* s_number.toInt();
	ASSERT_EQ(components.getFeatureCount(), number);

	//Check M3
	sys = (DM::GDALSystem*) m3->getOutPortData("city");
	components.setCurrentGDALSystem(sys);

	s_number = QString(FEATURES);
	number = 3* s_number.toInt();
	ASSERT_EQ(components.getFeatureCount(), number);

	ASSERT_TRUE(cleanup(sim));
}
#endif

#ifdef EXPANDING
TEST_F(TestGDALModules,UpdateTest) {
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
	DM::Logger(DM::Standard) << "Create System";

	DM::Simulation * sim = startup();
	DM::SimulationConfig conf;
	conf.setCoordinateSystem(DEFAULTEPSG);
	sim->setSimulationConfig(conf);
	QDir dir("./");
	sim->registerModulesFromDirectory(dir);

	DM::Module * m1 = sim->addModule("GDALAddComponent");
	m1->setParameterValue("elements", FEATURES);

	DM::Module * m2 = sim->addModule("UpdateAllComponents");
	m2->setParameterValue("elements", FEATURES);
	m2->init();
	sim->addLink(m1, "city", m2, "city");

	sim->run();

	ASSERT_TRUE(cleanup(sim));
}
#endif

#ifdef BRANCHMODIFY
TEST_F(TestGDALModules,BranchModify) {
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
	DM::Logger(DM::Standard) << "Create System";

	DM::Simulation * sim = startup();
	DM::SimulationConfig conf;
	conf.setCoordinateSystem(DEFAULTEPSG);
	sim->setSimulationConfig(conf);
	QDir dir("./");
	sim->registerModulesFromDirectory(dir);

	DM::Module * m1 = sim->addModule("GDALAddComponent");
	m1->setParameterValue("elements", FEATURES);
	m1->setSuccessorMode(true);

	DM::Module * m2 = sim->addModule("UpdateAllComponents");

	m2->setParameterValue("elements", FEATURES);
	m2->init();
	sim->addLink(m1, "city", m2, "city");

	sim->run();

	//Check M2
	DM::GDALSystem * sys = (DM::GDALSystem*) m2->getOutPortData("city");
	DM::ViewContainer components = DM::ViewContainer("component", DM::NODE, DM::READ);
	components.setCurrentGDALSystem(sys);

	QString s_number = QString(FEATURES);
	int number = s_number.toInt();
	ASSERT_EQ(components.getFeatureCount(), number);

	ASSERT_TRUE(cleanup(sim));
}
#endif

#ifdef WRITEATTRIBUTES
TEST_F(TestGDALModules,WriteAttributes) {

	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
	DM::Logger(DM::Standard) << "Create System";

	DM::Simulation * sim = startup();
	DM::SimulationConfig conf;
	conf.setCoordinateSystem(DEFAULTEPSG);
	sim->setSimulationConfig(conf);
	QDir dir("./");
	sim->registerModulesFromDirectory(dir);
	DM::Module * m1 = sim->addModule("GDALAddComponent");
	m1->setParameterValue("elements", FEATURES);
	m1->setParameterValue("add_attributes", "1");

	sim->run();

	DM::GDALSystem * sys = (DM::GDALSystem*) m1->getOutPortData("city");
	ASSERT_TRUE(sys);
	DM::ViewContainer components = DM::ViewContainer("component", DM::NODE, DM::READ);
	components.setCurrentGDALSystem(sys);

	OGRFeature * f;
	components.resetReading();
	while (f = components.getNextFeature()) {
		ASSERT_TRUE(f);
		double double_val = f->GetFieldAsDouble("double_attribute");
		ASSERT_FLOAT_EQ( double_val, 10.5 );

		std::vector<double> ress_vec;
		DM::DMFeature::GetDoubleList(f,"double_vector_attribute", ress_vec );
		DM::Logger(DM::Error) << ress_vec.size();
		ASSERT_EQ( ress_vec.size(), 3 );
		ASSERT_FLOAT_EQ( ress_vec[2], 30.5 );
		return;
	}

	ASSERT_TRUE(cleanup(sim));

}
#endif

#ifdef TESTLINKS
TEST_F(TestGDALModules,TestGDALLinks) {

	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
	DM::Logger(DM::Standard) << "Create System";

	DM::Simulation * sim =startup();
	DM::SimulationConfig conf;
	conf.setCoordinateSystem(DEFAULTEPSG);
	sim->setSimulationConfig(conf);
	QDir dir("./");
	sim->registerModulesFromDirectory(dir);
	DM::Module * m1 = sim->addModule("GDALAddComponentViewContainer");
	m1->setParameterValue("elements", FEATURES);

	DM::Module * m2 = sim->addModule("GDALLinkedComponent");
	m2->setParameterValue("elements", FEATURES);
	m2->setParameterValue("append", "1");
	m2->init();
	sim->addLink(m1, "city", m2, "city");

	DM::Module * m3 = sim->addModule("GDALValidateLinks");
	m3->init();
	sim->addLink(m2, "city", m3, "city");
	sim->run();
	ASSERT_EQ(sim->getSimulationStatus(), DM::SIM_OK);

	ASSERT_TRUE(cleanup(sim));
}
#endif

#ifdef TESTSYSTEMSETTINGS
TEST_F(TestGDALModules,TestSystemSettings) {
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
	DM::Logger(DM::Standard) << "Create System";


	DM::SimulationConfig conf;
	conf.setCoordinateSystem(DEFAULTEPSG);
	conf.setWorkingDir(QString(QDir::tempPath()+"/dynamind_test").toStdString());
	conf.setKeepSystems(true);
	DM::Simulation * sim = startup(QString::fromStdString(conf.getWorkingDir()));
	sim->setSimulationConfig(conf);
	QDir dir("./");
	sim->registerModulesFromDirectory(dir);

	DM::Module * m1 = sim->addModule("GDALAddComponent");
	m1->setParameterValue("elements", FEATURES);
	m1->setSuccessorMode(true);

	DM::Module * m2 = sim->addModule("UpdateAllComponents");

	m2->setParameterValue("elements", FEATURES);
	m2->init();
	sim->addLink(m1, "city", m2, "city");

	sim->run();

	//Check M2
	DM::GDALSystem * sys = (DM::GDALSystem*) m2->getOutPortData("city");
	DM::ViewContainer components = DM::ViewContainer("component", DM::NODE, DM::READ);
	components.setCurrentGDALSystem(sys);

	QString s_number = QString(FEATURES);
	int number = s_number.toInt();
	ASSERT_EQ(components.getFeatureCount(), number);

	ASSERT_FALSE(cleanup(sim, QString::fromStdString(conf.getWorkingDir())));
}
#endif

#ifdef TESTSAVELOAD
TEST_F(TestGDALModules,TestSaveLoad) {
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Error);
	DM::Logger(DM::Standard) << "Create System";


	DM::SimulationConfig conf;
	conf.setCoordinateSystem(DEFAULTEPSG);
	conf.setWorkingDir(QString(QDir::tempPath()+"/dynamind_test").toStdString());
	conf.setKeepSystems(true);
	DM::Simulation * sim = startup(QString::fromStdString(conf.getWorkingDir()));
	sim->setSimulationConfig(conf);
	QDir dir("./");
	sim->registerModulesFromDirectory(dir);

	DM::Module * m1 = sim->addModule("GDALAddComponent");
	m1->setParameterValue("elements", FEATURES);
	m1->setSuccessorMode(true);

	DM::Module * m2 = sim->addModule("UpdateAllComponents");
	std::string me2_name = m2->getName();

	m2->setParameterValue("elements", FEATURES);
	m2->init();
	sim->addLink(m1, "city", m2, "city");

	sim->run();

	//Check M2
	DM::GDALSystem * sys = (DM::GDALSystem*) m2->getOutPortData("city");
	DM::ViewContainer components = DM::ViewContainer("component", DM::NODE, DM::READ);
	components.setCurrentGDALSystem(sys);

	QString s_number = QString(FEATURES);
	int number = s_number.toInt();
	ASSERT_EQ(components.getFeatureCount(), number);

	std::string simulation_file = QString(QDir::tempPath()+"/testfile.dyn").toStdString();
	sim->writeSimulation(simulation_file);
	ASSERT_FALSE(cleanup(sim, QString::fromStdString(conf.getWorkingDir())));

	sim = startup(QString::fromStdString(conf.getWorkingDir()));
	sim->registerModulesFromDirectory(dir);
	sim->loadSimulation(simulation_file);

	sim->run();
	m2 = 0;
	foreach(DM::Module * m, sim->getModules()) {
		if (m->getName() == me2_name) {
			m2 = m;
		}
	}

	//Check M2
	sys = (DM::GDALSystem*) m2->getOutPortData("city");
	DM::ViewContainer components1 = DM::ViewContainer("component", DM::NODE, DM::READ);
	components1.setCurrentGDALSystem(sys);

	s_number = QString(FEATURES);
	number = s_number.toInt();
	ASSERT_EQ(components1.getFeatureCount(), number);

	ASSERT_FALSE(cleanup(sim, QString::fromStdString(conf.getWorkingDir())));
}
#endif







