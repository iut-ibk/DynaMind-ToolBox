#include "unittestwaterbalance.h"
#include <dmsimulation.h>
#include <dmlog.h>
#include <dmlogger.h>
#include <dmlogsink.h>

void UnitTestWaterBalance::SetUp()
{
    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);
    DM::Logger(DM::Standard) << "Load Native Module";
    sim = new DM::Simulation();
    sim->registerModulesFromDirectory(QDir("./Modules"));
}

void UnitTestWaterBalance::TearDown()
{
    delete sim;
}

TEST_F(UnitTestWaterBalance,setParameterInModule)
{
    ASSERT_TRUE(sim!=0);

    DM::Module * m = sim->addModule("WaterBalanceHouseHold");
    sim->run();

    ASSERT_TRUE(m!=0);
}


TEST_F(UnitTestWaterBalance, LoadModule) {

}
