#include "testcgal.h"

#include <dm.h>
#include <tbvectordata.h>
#include <cgalgeometry.h>

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
}
