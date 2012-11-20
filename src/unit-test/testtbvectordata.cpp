#include "testtbvectordata.h"
#include <tbvectordata.h>
#include <dm.h>

namespace {
    TEST_F(TestTBVectorData,calculateArea){
        DM::System * sys = new DM::System();

        DM::Node * n1 = sys->addNode(DM::Node(1,1,0));
        DM::Node * n2 = sys->addNode(DM::Node(2,1,0));
        DM::Node * n3 = sys->addNode(DM::Node(2,2,0));
        DM::Node * n4 = sys->addNode(DM::Node(1,2,0));

        std::vector<DM::Node * > nodes;
        nodes.push_back(n1);
        nodes.push_back(n2);
        nodes.push_back(n3);
        nodes.push_back(n4);
        nodes.push_back(n1);

        DM::Face * f = sys->addFace(nodes);

        double area = TBVectorData::CalculateArea(sys,f);
        EXPECT_DOUBLE_EQ(1, area);

    }
    TEST_F(TestTBVectorData,calculateAreaWithHole){
        DM::System * sys = new DM::System();

        DM::Node * n1 = sys->addNode(DM::Node(1,1,0));
        DM::Node * n2 = sys->addNode(DM::Node(3,1,0));
        DM::Node * n3 = sys->addNode(DM::Node(3,3,0));
        DM::Node * n4 = sys->addNode(DM::Node(1,3,0));


        DM::Node * n1_h = sys->addNode(DM::Node(1.5,1.5,0));
        DM::Node * n2_h = sys->addNode(DM::Node(2.5,1.5,0));
        DM::Node * n3_h = sys->addNode(DM::Node(2.5,2.5,0));
        DM::Node * n4_h = sys->addNode(DM::Node(1.5,2.5,0));


        std::vector<DM::Node * > nodes;
        nodes.push_back(n1);
        nodes.push_back(n2);
        nodes.push_back(n3);
        nodes.push_back(n4);
        nodes.push_back(n1);

        std::vector<DM::Node * > nodes_h;
        nodes_h.push_back(n1_h);
        nodes_h.push_back(n2_h);
        nodes_h.push_back(n3_h);
        nodes_h.push_back(n4_h);
        nodes_h.push_back(n1_h);

        DM::Face * f = sys->addFace(nodes);
        f->addHole(nodes_h);

        double area = TBVectorData::CalculateArea(sys,f);

        EXPECT_DOUBLE_EQ(3, area);

    }
    TEST_F(TestTBVectorData,calculateArea3d){
        DM::System * sys = new DM::System();

        DM::Node * n1 = sys->addNode(DM::Node(1,0,1));
        DM::Node * n2 = sys->addNode(DM::Node(2,0,1));
        DM::Node * n3 = sys->addNode(DM::Node(2,0,2));
        DM::Node * n4 = sys->addNode(DM::Node(1,0,2));

        std::vector<DM::Node * > nodes;
        nodes.push_back(n1);
        nodes.push_back(n2);
        nodes.push_back(n3);
        nodes.push_back(n4);
        nodes.push_back(n1);

        DM::Face * f = sys->addFace(nodes);

        double area = TBVectorData::CalculateArea(sys,f);
        EXPECT_DOUBLE_EQ(1, area);

    }
    TEST_F(TestTBVectorData,calculateAreaWithHole3D){
        DM::System * sys = new DM::System();

        DM::Node * n1 = sys->addNode(DM::Node(1,0,1));
        DM::Node * n2 = sys->addNode(DM::Node(3,0,1));
        DM::Node * n3 = sys->addNode(DM::Node(3,0,3));
        DM::Node * n4 = sys->addNode(DM::Node(1,0,3));


        DM::Node * n1_h = sys->addNode(DM::Node(1.5,0,1.5));
        DM::Node * n2_h = sys->addNode(DM::Node(2.5,0,1.5));
        DM::Node * n3_h = sys->addNode(DM::Node(2.5,0,2.5));
        DM::Node * n4_h = sys->addNode(DM::Node(1.5,0,2.5));


        std::vector<DM::Node * > nodes;
        nodes.push_back(n1);
        nodes.push_back(n2);
        nodes.push_back(n3);
        nodes.push_back(n4);
        nodes.push_back(n1);

        std::vector<DM::Node * > nodes_h;
        nodes_h.push_back(n1_h);
        nodes_h.push_back(n2_h);
        nodes_h.push_back(n3_h);
        nodes_h.push_back(n4_h);
        nodes_h.push_back(n1_h);

        DM::Face * f = sys->addFace(nodes);
        f->addHole(nodes_h);

        double area = TBVectorData::CalculateArea(sys,f);

        EXPECT_DOUBLE_EQ(3, area);

    }
    TEST_F(TestTBVectorData,calculateCentroid){
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

        DM::Face * f = sys->addFace(nodes);


        DM::Node c = TBVectorData::CaclulateCentroid(sys, f);

        EXPECT_DOUBLE_EQ(2.0, c.getX());
        EXPECT_DOUBLE_EQ(2.0, c.getY());

        std::reverse(nodes.begin(), nodes.end());
        DM::Face * f1 = sys->addFace(nodes);
        c = TBVectorData::CaclulateCentroid(sys, f1);
       EXPECT_DOUBLE_EQ(2.0, c.getX());
        EXPECT_DOUBLE_EQ(2.0, c.getY());


    }
}
