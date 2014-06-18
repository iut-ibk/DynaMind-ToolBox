#include "testgdal.h"
#include "iostream"


#include <dmgdalsystem.h>
#include <dmsystem.h>
#include <dmlogger.h>
#include <dmlogsink.h>
#include <dmcomponent.h>
#include <dmview.h>
#include <dmviewcontainer.h>
#include <dmattribute.h>

#define SPEEDTEST
#define CONTAINERCREATE
#define CONTAINERCREATEATTRIBUTE
#define TESTSTATES

#define  ELEMENTS 1000000

#ifdef SPEEDTEST
/**
 * @brief Testing the insert speed as qualtiy control
 * please add test results below
 * Elementes		time in msec		comments
 *   1.000.000			   6.844		iMac including state_id;
 *  10.000.000			  69.709		iMac including state_id;
 * 100.000.000			 594.004		iMac office with uuid; sync 10.000
 *  10.000.000			  50.656		iMac office with uuid; sync 10.000
 *  10.000.000			  50.708		iMac office with uuid; sync 100.000
 *   1.000.000		       6.931		MacBook with uuid; sync 10.000
 *   1.000.000		       3.627		MacBook without uuid; sync 10.000
 */
TEST_F(TestGDAL,TestInsertSpeed) {

	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Debug);
	DM::Logger(DM::Standard) << "Create System";

	DM::View testview("TestComponents", DM::COMPONENT, DM::WRITE);


	DM::GDALSystem sys;
	sys.updateSystemView(testview);

	QTime myTimer;
	myTimer.start();

	for (int i = 0; i < ELEMENTS; i++) {
		sys.createFeature(testview);
	}
	sys.syncFeatures(testview);
	DM::Logger(DM::Standard) << myTimer.elapsed();

	ASSERT_EQ( sys.getOGRLayer(DM::View("TestComponents", DM::COMPONENT, DM::WRITE))->GetFeatureCount(), ELEMENTS);
}
#endif

#ifdef CONTAINERCREATE
TEST_F(TestGDAL, TestViewContainerCreate) {
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Debug);
	DM::Logger(DM::Standard) << "Create System";
	DM::ViewContainer * TestComponents = new DM::ViewContainer("TestComponents", DM::COMPONENT, DM::WRITE);

	DM::GDALSystem sys;
	sys.updateSystemView(*TestComponents);

	TestComponents->setCurrentGDALSystem(&sys);

	QTime myTimer;
	myTimer.start();
	for (int i = 0; i < ELEMENTS; i++)
		ASSERT_TRUE(TestComponents->createFeature() != NULL) ;
	delete TestComponents;
	DM::Logger(DM::Standard) << myTimer.elapsed();

	ASSERT_EQ( sys.getOGRLayer(DM::View("TestComponents", DM::COMPONENT, DM::WRITE))->GetFeatureCount(), ELEMENTS);

}
#endif

#ifdef CONTAINERCREATEATTRIBUTE
TEST_F(TestGDAL, TestViewContainerInsertAttribute) {
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Debug);

	DM::ViewContainer * TestComponents = new DM::ViewContainer("TestComponents", DM::COMPONENT, DM::WRITE);
	TestComponents->addAttribute("some_attribute", DM::Attribute::STRING, DM::WRITE);

	DM::GDALSystem sys;
	sys.updateSystemView(*TestComponents);

	TestComponents->setCurrentGDALSystem(&sys);

	OGRFeature * f = TestComponents->createFeature();
	f->SetField("some_attribute", "this is a test");
	TestComponents->syncFeatures();

	ASSERT_EQ( TestComponents->getFeatureCount(), 1 ) ;
	ASSERT_STREQ( TestComponents->getFeature(1)->GetFieldAsString("some_attribute"), "this is a test");

	delete TestComponents;
}
#endif


/**
  write  10.000.000   69.612
  read			      4.291 (only count)
  */
#ifdef TESTSTATES
TEST_F(TestGDAL, TestStates) {
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);

	DM::ViewContainer * TestComponents = new DM::ViewContainer("TestComponents", DM::COMPONENT, DM::WRITE);
	TestComponents->addAttribute("some_attribute", DM::Attribute::STRING, DM::WRITE);

	DM::GDALSystem sys;
	sys.updateSystemView(*TestComponents);

	TestComponents->setCurrentGDALSystem(&sys);

	QTime myTimer;
	myTimer.start();


	for (int i = 0; i < ELEMENTS; i++)
		ASSERT_TRUE(TestComponents->createFeature() != NULL) ;
	TestComponents->syncFeatures();
	DM::Logger(DM::Standard) <<  "write " << ELEMENTS << " " << myTimer.elapsed();

	myTimer.restart();
	ASSERT_EQ( TestComponents->getFeatureCount(), ELEMENTS) ;
	DM::Logger(DM::Standard)<< "count"  << myTimer.elapsed();

	myTimer.restart();
	//Create new State
	DM::GDALSystem sys_2 = DM::GDALSystem(sys);
	TestComponents->setCurrentGDALSystem(&sys_2);
	for (int i = 0; i < ELEMENTS; i++)
		ASSERT_TRUE(TestComponents->createFeature() != NULL) ;
	TestComponents->syncFeatures();
	DM::Logger(DM::Standard)<<  "write " << ELEMENTS << " " <<myTimer.elapsed();
	myTimer.restart();
	ASSERT_EQ( TestComponents->getFeatureCount(), ELEMENTS*2) ;
	DM::Logger(DM::Standard) << "count" << myTimer.elapsed();

	//old sys
	myTimer.restart();
	TestComponents->setCurrentGDALSystem(&sys);
	ASSERT_EQ( TestComponents->getFeatureCount(), ELEMENTS ) ;
	DM::Logger(DM::Standard) << "count" << myTimer.elapsed();
	delete TestComponents;
}
#endif
