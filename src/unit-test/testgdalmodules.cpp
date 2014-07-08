#include "testgdalmodules.h"

#include <dmsimulation.h>
#include <dmlogger.h>
#include <dmlog.h>
#include <dmlogsink.h>
#include <ogrsf_frmts.h>
#include <dmviewcontainer.h>
#include <dmsystem.h>

#define FEATURES "100000"
#define FEATURES_2 "200000"

/*#define SPEEDTESTDM
#define SPEEDTEST
#define CONNECTIONTEST
#define BRANCHINGTEST
#define EXPANDING
#define BRANCHMODIFY*/

#ifdef SPEEDTESTDM
TEST_F(TestGDALModules,TestInsertSpeed_DM) {

	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Debug);
	DM::Logger(DM::Standard) << "Create System";

	DM::Simulation sim;
	QDir dir("./");
	sim.registerModulesFromDirectory(dir);
	sim.addModule("CreateNodes")->setParameterValue("elements", FEATURES);;
	sim.run();
}
#endif

#ifdef SPEEDTEST
TEST_F(TestGDALModules,TestInsertSpeed) {

	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Debug);
	DM::Logger(DM::Standard) << "Create System";

	DM::Simulation sim;
	QDir dir("./");
	sim.registerModulesFromDirectory(dir);
	sim.addModule("GDALAddComponent")->setParameterValue("elements", FEATURES);
	sim.run();
}
#endif

#ifdef CONNECTIONTEST
TEST_F(TestGDALModules,ConnectionTest) {

	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Debug);
	DM::Logger(DM::Standard) << "Create System";

	DM::Simulation sim;
	QDir dir("./");
	sim.registerModulesFromDirectory(dir);
	DM::Module * m1 = sim.addModule("GDALAddComponent");
	m1->setParameterValue("elements", FEATURES);

	DM::Module * m2 = sim.addModule("GDALAddComponent");
	m2->setParameterValue("elements", FEATURES);
	m2->setParameterValue("append", "1");
	m2->init();
	sim.addLink(m1, "city", m2, "city");
	sim.run();

	DM::GDALSystem * sys = (DM::GDALSystem*) m2->getOutPortData("city");
	DM::ViewContainer components = DM::ViewContainer("component", DM::NODE, DM::READ);
	components.setCurrentGDALSystem(sys);

	QString s_number = QString(FEATURES);
	int number = 2* s_number.toInt();
	ASSERT_EQ(components.getFeatureCount(), number);
}
#endif

#ifdef BRANCHINGTEST
TEST_F(TestGDALModules,BranchingTest) {

	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Debug);
	DM::Logger(DM::Standard) << "Create System";

	DM::Simulation sim;
	QDir dir("./");
	sim.registerModulesFromDirectory(dir);

	DM::Module * m1 = sim.addModule("GDALAddComponent");
	m1->setParameterValue("elements", FEATURES);

	DM::Module * m2 = sim.addModule("GDALAddComponent");
	m2->setParameterValue("elements", FEATURES);
	m2->setParameterValue("append", "1");
	m2->init();
	sim.addLink(m1, "city", m2, "city");

	DM::Module * m3 = sim.addModule("GDALAddComponent");
	m3->setParameterValue("elements", FEATURES_2);
	m3->setParameterValue("append", "1");
	m3->init();
	sim.addLink(m1, "city", m3, "city");

	sim.run();

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
}
#endif

#ifdef EXPANDING
TEST_F(TestGDALModules,UpdateTest) {
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);
	DM::Logger(DM::Standard) << "Create System";

	DM::Simulation sim;
	QDir dir("./");
	sim.registerModulesFromDirectory(dir);

	DM::Module * m1 = sim.addModule("GDALAddComponent");
	m1->setParameterValue("elements", FEATURES);

	DM::Module * m2 = sim.addModule("UpdateAllComponents");
	m2->setParameterValue("elements", FEATURES);
	m2->init();
	sim.addLink(m1, "city", m2, "city");

	sim.run();
}
#endif

#ifdef BRANCHMODIFY
TEST_F(TestGDALModules,BranchModify) {
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);
	DM::Logger(DM::Standard) << "Create System";

	DM::Simulation sim;
	QDir dir("./");
	sim.registerModulesFromDirectory(dir);

	DM::Module * m1 = sim.addModule("GDALAddComponent");
	m1->setParameterValue("elements", FEATURES);
	m1->setSuccessorMode(true);

	DM::Module * m2 = sim.addModule("UpdateAllComponents");

	m2->setParameterValue("elements", FEATURES);
	m2->init();
	sim.addLink(m1, "city", m2, "city");

	sim.run();

	//Check M2
	DM::GDALSystem * sys = (DM::GDALSystem*) m2->getOutPortData("city");
	DM::ViewContainer components = DM::ViewContainer("component", DM::NODE, DM::READ);
	components.setCurrentGDALSystem(sys);

	QString s_number = QString(FEATURES);
	int number = s_number.toInt();
	ASSERT_EQ(components.getFeatureCount(), number);
}
#endif






