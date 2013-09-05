#include "testtbvectordata.h"
#include <tbvectordata.h>
#include <dm.h>
#include <math.h>

namespace {



TEST_F(TestTBVectorData,minNode){
    DM::System * sys = new DM::System();

    DM::Node * n1 = sys->addNode(DM::Node(1,1,0));
    DM::Node * n2 = sys->addNode(DM::Node(3,1,0));
    DM::Node * n3 = sys->addNode(DM::Node(3,3,0));
    DM::Node * n4 = sys->addNode(DM::Node(1,3,0));

    std::vector<DM::Node * > nodes;
    nodes.push_back(n1);
    nodes.push_back(n2);
    nodes.push_back(n3);
    nodes.push_back(n4);
    nodes.push_back(n1);

    DM::Node c = TBVectorData::MinCoordinates(nodes);
    EXPECT_DOUBLE_EQ(1.0, c.getX());
    EXPECT_DOUBLE_EQ(1.0, c.getY());
    EXPECT_DOUBLE_EQ(0.0, c.getZ());
	
	delete sys;
}


TEST_F(TestTBVectorData,AngelBetweenVectors){
	   EXPECT_DOUBLE_EQ(0,TBVectorData::AngelBetweenVectors(DM::Node(1,0,0), DM::Node(1,0,0)) *180./M_PI);
	   EXPECT_DOUBLE_EQ(90,TBVectorData::AngelBetweenVectors(DM::Node(1,0,0), DM::Node(0,1,0))*180./M_PI);
	   EXPECT_DOUBLE_EQ(180,TBVectorData::AngelBetweenVectors(DM::Node(1,0,0), DM::Node(-1,0,0))*180./M_PI);
	   EXPECT_DOUBLE_EQ(90,TBVectorData::AngelBetweenVectors(DM::Node(1,0,0), DM::Node(0,-1,0))*180./M_PI);
}
}
