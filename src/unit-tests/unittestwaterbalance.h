#ifndef UNITTESTWATERBALANCE_H
#define UNITTESTWATERBALANCE_H


#include <gtest/gtest.h>
#include <dmsimulation.h>
#include <dmmodule.h>

class UnitTestWaterBalance : public ::testing::Test
{
public:
    DM::Simulation *sim;

protected:
        UnitTestWaterBalance()
        {
            sim = 0;
        }
        virtual ~UnitTestWaterBalance(){}
        virtual void SetUp();
        virtual void TearDown();

};

#endif // UNITTESTWATERBALANCE_H
