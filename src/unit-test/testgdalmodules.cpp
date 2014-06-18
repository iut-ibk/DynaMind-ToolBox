#include "testgdalmodules.h"

#include <dmsimulation.h>
#include <dmlogger.h>
#include <dmlog.h>
#include <dmlogsink.h>



TEST_F(TestGDALModules,TestInsertSpeed) {

	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Debug);
	DM::Logger(DM::Standard) << "Create System";

	DM::Simulation sim;
	QDir dir("./");
	sim.registerModulesFromDirectory(dir);
	sim.addModule("GDALAddComponent");
	sim.run();
}

