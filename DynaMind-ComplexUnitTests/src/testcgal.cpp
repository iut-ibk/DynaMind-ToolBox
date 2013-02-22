#include "testcgal.h"

#include <dm.h>
#include <tbvectordata.h>
#include <cgalgeometry.h>
#include <cgalsearchoperations.h>
#include "cgalskeletonisation.h"
#include <dmlog.h>
namespace {
TEST_F(TestCGAL, triangulation){
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
    std::vector<DM::Node> tnodes = DM::CGALGeometry::FaceTriangulation(sys, f);

    //Create Triangles
    std::vector<DM::Node* > tnodes_p;
    foreach(DM::Node n, tnodes) {
        tnodes_p.push_back(sys->addNode(n));
    }

    int n_triangles = tnodes_p.size();
    std::vector<DM::Face* > triangles;
    for (int i = 0; i < n_triangles/3; i++) {
        std::vector<DM::Node * > ns;
        for (int j = 0; j < 3; j++) {
            ns.push_back(tnodes_p[j+i*3]);
        }
        ns.push_back(tnodes_p[i*3]);
        triangles.push_back(sys->addFace(ns));
    }

    //Compare Areas

    //Area
    double area_ref = TBVectorData::CalculateArea(sys, f);

    double a_triangles = 0;
    foreach (DM::Face * f_t, triangles) {
        a_triangles+=TBVectorData::CalculateArea(sys, f_t);
    }

    EXPECT_DOUBLE_EQ(area_ref, a_triangles);
}
TEST_F(TestCGAL, triangulation_with_holes){
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

    std::vector<DM::Node> tnodes = DM::CGALGeometry::FaceTriangulation(sys, f);

    //Create Triangles
    std::vector<DM::Node* > tnodes_p;
    foreach(DM::Node n, tnodes) {
        tnodes_p.push_back(sys->addNode(n));
    }

    int n_triangles = tnodes_p.size();
    std::vector<DM::Face* > triangles;
    for (int i = 0; i < n_triangles/3; i++) {
        std::vector<DM::Node * > ns;
        for (int j = 0; j < 3; j++) {
            ns.push_back(tnodes_p[j+i*3]);
        }
        ns.push_back(tnodes_p[i*3]);
        triangles.push_back(sys->addFace(ns));
    }

    //Compare Areas

    //Area
    double area_ref = TBVectorData::CalculateArea(sys, f);

    double a_triangles = 0;
    foreach (DM::Face * f_t, triangles) {
        a_triangles+=TBVectorData::CalculateArea(sys, f_t);
    }

    EXPECT_DOUBLE_EQ(area_ref, a_triangles);
}

TEST_F(TestCGAL, regular_triangulation){
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
    std::vector<int>  nodeids;
    std::vector<DM::Node> tnodes = DM::CGALGeometry::RegularFaceTriangulation(sys, f, nodeids,0.5);

    //Create Triangles
    std::vector<DM::Node* > tnodes_p;
    foreach(DM::Node n, tnodes) {
        tnodes_p.push_back(sys->addNode(n));
    }

    int n_triangles = nodeids.size()/3;
    std::vector<DM::Face* > triangles;
    for (int i = 0; i < n_triangles; i++) {
        std::vector<DM::Node * > ns;
        for (int j = 0; j < 3; j++) {
            ns.push_back(tnodes_p[nodeids[j+i*3]]);
        }
        ns.push_back(ns[0]);
        triangles.push_back(sys->addFace(ns));
    }

    //Compare Areas

    //Area
    double area_ref = TBVectorData::CalculateArea(sys, f);

    double a_triangles = 0;
    foreach (DM::Face * f_t, triangles) {
        a_triangles+=TBVectorData::CalculateArea(sys, f_t);
    }

    EXPECT_DOUBLE_EQ(area_ref, a_triangles);
}

TEST_F(TestCGAL, intersectionTest){
    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Debug);
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


    DM::Node * n1_1 = sys->addNode(DM::Node(1.5,1.5,0));
    DM::Node * n2_1 = sys->addNode(DM::Node(2.5,1.5,0));
    DM::Node * n3_1 = sys->addNode(DM::Node(2.5,2.5,0));
    DM::Node * n4_1 = sys->addNode(DM::Node(1.5,2.5,0));

    std::vector<DM::Node * > nodes1;
    nodes1.push_back(n1_1);
    nodes1.push_back(n2_1);
    nodes1.push_back(n3_1);
    nodes1.push_back(n4_1);
    nodes1.push_back(n1_1);

    DM::Face * f1 = sys->addFace(nodes);
    DM::Face * f2 = sys->addFace(nodes1);


    std::vector<DM::Node> interested;
    interested    = DM::CGALGeometry::IntersectFace(sys, f1, f2);

    std::vector<DM::Node*> nodes_i;
    if (interested.size() < 3)
        return;
    foreach (DM::Node n, interested) {
        nodes_i.push_back(sys->addNode(n.getX(), n.getY(), n.getZ()));
    }

    nodes_i.push_back(nodes_i[0]);

    double area_i = TBVectorData::CalculateArea(nodes_i);
    EXPECT_DOUBLE_EQ(0.25, area_i);
}

TEST_F(TestCGAL, dointersectionTest){
    ostream *out = &cout;
    DM::Log::init(new DM::OStreamLogSink(*out), DM::Debug);
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


    DM::Node * n1_1 = sys->addNode(DM::Node(1.5,1.5,0));
    DM::Node * n2_1 = sys->addNode(DM::Node(2.5,1.5,0));
    DM::Node * n3_1 = sys->addNode(DM::Node(2.5,2.5,0));
    DM::Node * n4_1 = sys->addNode(DM::Node(1.5,2.5,0));

    std::vector<DM::Node * > nodes1;
    nodes1.push_back(n1_1);
    nodes1.push_back(n2_1);
    nodes1.push_back(n3_1);
    nodes1.push_back(n4_1);
    nodes1.push_back(n1_1);


    DM::Node * n1_2 = sys->addNode(DM::Node(3.5,3.5,0));
    DM::Node * n2_2 = sys->addNode(DM::Node(4.5,3.5,0));
    DM::Node * n3_2 = sys->addNode(DM::Node(4.5,4.5,0));
    DM::Node * n4_2 = sys->addNode(DM::Node(3.5,4.5,0));

    std::vector<DM::Node * > nodes2;
    nodes2.push_back(n1_2);
    nodes2.push_back(n2_2);
    nodes2.push_back(n3_2);
    nodes2.push_back(n4_2);
    nodes2.push_back(n1_2);


    ASSERT_TRUE(DM::CGALGeometry::DoFacesInterect(nodes, nodes1));
    ASSERT_FALSE(DM::CGALGeometry::DoFacesInterect(nodes, nodes2));

}

TEST_F(TestCGAL, simpleNodeSearch){
    DM::System * sys = new DM::System();

    DM::Node * n1_1 = sys->addNode(DM::Node(1,1,0));
    DM::Node * n2_1 = sys->addNode(DM::Node(1,2,0));
    DM::Node * n3_1 = sys->addNode(DM::Node(2,3,0));
    DM::Node * n4_1 = sys->addNode(DM::Node(0,4,0));

    std::vector<DM::Node * > nodes1;
    nodes1.push_back(n4_1);
    nodes1.push_back(n1_1);
    nodes1.push_back(n2_1);
    nodes1.push_back(n3_1);


    DM::Node searchNode(1,6,0);

    std::vector<DM::Node> rn = CGALSearchOperations::NearestPoints(nodes1, &searchNode);
    EXPECT_GT(rn.size(), 0);
    DM::Node n1 = rn[0];
    EXPECT_DOUBLE_EQ(n1.getX(), 0);
    EXPECT_DOUBLE_EQ(n1.getY(), 4);
}

TEST_F(TestCGAL, skeletonisation){
    DM::System * sys = new DM::System();

    DM::Node * n1_1 = sys->addNode(DM::Node(0,1,0));
    DM::Node * n1_2 = sys->addNode(DM::Node(0,2,0));
    DM::Node * n1_3 = sys->addNode(DM::Node(4,2,0));
    DM::Node * n1_4 = sys->addNode(DM::Node(4,0,0));
    DM::Node * n1_5 = sys->addNode(DM::Node(2,0,0));
    DM::Node * n1_6 = sys->addNode(DM::Node(2,1,0));

    std::vector<DM::Node * > nodes1;
    nodes1.push_back(n1_1);
    nodes1.push_back(n1_2);
    nodes1.push_back(n1_3);
    nodes1.push_back(n1_4);
    nodes1.push_back(n1_5);
    nodes1.push_back(n1_6);
    nodes1.push_back(n1_1);

    DM::Face * f = sys->addFace(nodes1);


    DM::System rn = DM::CGALSkeletonisation::StraightSkeletonisation(sys, f,30);
    /*EXPECT_GT(rn.size(), 0);
    DM::Node n1 = rn[0];
    EXPECT_DOUBLE_EQ(n1.getX(), 0);
    EXPECT_DOUBLE_EQ(n1.getY(), 4);*/
}

}
