#include "testgdalpython.h"

#include <dmsimulation.h>
#include <dmlogger.h>
#include <dmlog.h>
#include <dmlogsink.h>
#include <ogrsf_frmts.h>
#include <dmviewcontainer.h>
#include <dmsystem.h>
#include <dmmoduleregistry.h>
#include <dmpythonenv.h>

#define LOADPYTHON


#ifdef LOADPYTHON
TEST_F(TestGDALPython,LoadPython) {


	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Debug);
	DM::Logger(DM::Standard) << "Create Simulation";
	//std::cout << QDir::currentPath().toStdString() << std::endl;
	DM::PythonEnv::getInstance()->addPythonPath(QDir::currentPath().toStdString());

	DM::Simulation sim;

	sim.registerModulesFromDefaultLocation();
	DM::Logger(DM::Debug) << "Loaded Modules";
	foreach (std::string m, sim.getModuleRegistry()->getRegisteredModules() ) {
		DM::Logger(DM::Debug) << m;
	}

	ASSERT_EQ(NULL,!sim.addModule("CreateGDALComponents"));
	sim.run();

}
#endif
