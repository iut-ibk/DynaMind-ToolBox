#include "testgdal.h"
#include "iostream"


#include <dmgdalsystem.h>
#include <dmsystem.h>
#include <dmlogger.h>
#include <dmlogsink.h>
#include <dmcomponent.h>
#include <dmview.h>


TEST_F(TestGDAL,HelloWorld) {

	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Debug);
	DM::Logger(DM::Standard) << "Create System";

	DM::View testview("test", DM::COMPONENT, DM::WRITE);


	DM::GDALSystem sys;
	sys.updateSystemView(testview);

	QTime myTimer;
	myTimer.start();

	for (int i = 0; i < 100000000; i++) {
		sys.createFeature(testview);
	}
	sys.syncFeatures(testview);
	DM::Logger(DM::Standard) << myTimer.elapsed();

/*	OGRLayer * lyr = sys.viewLayer[testview.getName()];
	lyr->ResetReading();

	OGRFeature *poFeature;

	lyr->ResetReading();
	while( (poFeature = lyr->GetNextFeature()) != NULL ) {
		DM::Logger(DM::Debug) << poFeature->GetFieldAsString(0);
	}
*/
}

//10.000.000 6931 with uuid
//1.000.000 6931 with uuid
//1.000.000 3627 without uuid
