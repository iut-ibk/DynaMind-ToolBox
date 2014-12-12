#include "unittestwaterbalance.h"
#include <dmsimulation.h>
#include <dmlog.h>
#include <dmlogger.h>
#include <dmlogsink.h>
#include <dmpythonenv.h>
#include "../DynaMind-Performance-Assessment/simplewaterbalance.h"
#include "../DynaMind-Performance-Assessment/waterbalance_household.h"

#define WATERBALANCE
#define WATERBALANCETANKS
//#define SIMPLEWATEBALANCE

void UnitTestWaterBalance::SetUp()
{
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Debug);
	DM::Logger(DM::Standard) << "Load Native Module";
	sim = new DM::Simulation();
	DM::PythonEnv::getInstance()->addPythonPath(".");
	sim->registerModulesFromDirectory(QDir("./Modules"));
	sim->registerModulesFromDirectory(QDir("./PythonModules/scripts"));
}

void UnitTestWaterBalance::TearDown()
{
	delete sim;
}

#ifdef WATERBALANCE
TEST_F(UnitTestWaterBalance,setParameterInModule)
{
	ASSERT_TRUE(sim!=0);

	WaterBalanceHouseHold wb;
	wb.setCd3_dir("/Users/christianurich/Documents/DynaMind-ToolBox/build/debug/output/Modules");
	wb.setEvapofile("/Users/christianurich/Documents/DynaMind-ToolBox/Data/Raindata/melb_rain.ixx");
	wb.setRainfile("/Users/christianurich/Documents/DynaMind-ToolBox/Data/Raindata/Evapotranspiration.ixx");
	wb.initmodel();
	wb.calculateRunoffAndDemand(1000,.25, .5, 4);
	std::vector<double> sw = wb.getStormwater_runoff();
	std::vector<double> non_p=  wb.getNon_potable_demand();
	wb.createTankOption(0, 2.5, sw,non_p);



	WaterBalanceHouseHold wb1;
	wb1.setCd3_dir("/Users/christianurich/Documents/DynaMind-ToolBox/build/debug/output/Modules");
	wb1.setEvapofile("/Users/christianurich/Documents/DynaMind-ToolBox/Data/Raindata/melb_rain_24.ixx");
	wb1.setRainfile("/Users/christianurich/Documents/DynaMind-ToolBox/Data/Raindata/melb_eva_24.ixx");
	wb1.initmodel();
	wb1.calculateRunoffAndDemand(1000,.25, .5, 4);
	sw = wb1.getStormwater_runoff();
	non_p=  wb1.getNon_potable_demand();
	wb1.createTankOption(0, 0, sw,non_p);

}
#endif

#ifdef WATERBALANCETANKS
TEST_F(UnitTestWaterBalance,testTanks)
{
	ASSERT_TRUE(sim!=0);

	WaterBalanceHouseHold wb;
	wb.setCd3_dir("/Users/christianurich/Documents/DynaMind-ToolBox/build/debug/output/Modules");
	wb.setEvapofile("/Users/christianurich/Documents/DynaMind-ToolBox/Data/Raindata/melb_rain.ixx");
	wb.setRainfile("/Users/christianurich/Documents/DynaMind-ToolBox/Data/Raindata/Evapotranspiration.ixx");
	wb.initmodel();
	wb.calculateRunoffAndDemand(1000,.25, .5, 4);
	std::vector<double> sw = wb.getStormwater_runoff();
	std::vector<double> non_p=  wb.getNon_potable_demand();

	for (int s = 0; s < 20; s++) {
		std::cout << s <<"\t" << wb.createTankOption(0, s, sw,non_p) << std::endl;
	}

}
#endif

#ifdef SIMPLEWATEBALANCE
TEST_F(UnitTestWaterBalance, LoadModule) {
	ASSERT_TRUE(sim!=0);
	SimpleWaterBalance  * m =  (SimpleWaterBalance *) sim->addModule("SimpleWaterBalance");
	m->setParameterValue("cd3_dir", "/Users/christianurich/Documents/DynaMind-ToolBox/build/debug/output/Modules");
	m->initmodel();
	sim->run();

	sim->addModule("CreateGDALComponents");
	ASSERT_TRUE(m!=0);
}
#endif
