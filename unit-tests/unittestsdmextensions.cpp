#include "unittestsdmextensions.h"

#include <dm.h>
#include <tbvectordata.h>
#include <cgalgeometry.h>
#include <cgalgeometry_p.h>
#include <cgalsearchoperations.h>
#include "cgalskeletonisation.h"
#include <dmlog.h>
#include <dmlogger.h>
#include <dmlogsink.h>
#include <print_utils.h>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Polyhedron_3.h>
#include <iostream>


namespace {

void addRectangleWithHole(DM::System* sys, DM::View v)
{

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


	DM::Node * n1_h = sys->addNode(DM::Node(1.25,1.25,0));
	DM::Node * n2_h = sys->addNode(DM::Node(1.75,1.25,0));
	DM::Node * n3_h = sys->addNode(DM::Node(1.75,1.75,0));
	DM::Node * n4_h = sys->addNode(DM::Node(1.25,1.75,0));

	std::vector<DM::Node * > nodes_h;
	nodes_h.push_back(n1_h);
	nodes_h.push_back(n2_h);
	nodes_h.push_back(n3_h);
	nodes_h.push_back(n4_h);
	nodes_h.push_back(n1_h);

	DM::Face * f1 = sys->addFace(nodes, v);
	f1->addHole(nodes_h);
}



TEST_F(UnitTestsDMExtensions,calculateCentroid){
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);
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


	DM::Node c = DM::CGALGeometry::CalculateCentroid(sys, f);

	EXPECT_DOUBLE_EQ(2.0, c.getX());
	EXPECT_DOUBLE_EQ(2.0, c.getY());

	std::reverse(nodes.begin(), nodes.end());
	DM::Face * f1 = sys->addFace(nodes);
	c = DM::CGALGeometry::CalculateCentroid(sys, f1);
	EXPECT_DOUBLE_EQ(2.0, c.getX());
	EXPECT_DOUBLE_EQ(2.0, c.getY());

	delete sys;
}

TEST_F(UnitTestsDMExtensions,calculateCentroid3D_XZ){
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);
	DM::System * sys = new DM::System();

	DM::Node * n1 = sys->addNode(DM::Node(1,0,1));
	DM::Node * n2 = sys->addNode(DM::Node(3,0,1));
	DM::Node * n3 = sys->addNode(DM::Node(3,0,3));
	DM::Node * n4 = sys->addNode(DM::Node(1,0,3));

	std::vector<DM::Node * > nodes;
	nodes.push_back(n1);
	nodes.push_back(n2);
	nodes.push_back(n3);
	nodes.push_back(n4);
	nodes.push_back(n1);

	DM::Face * f = sys->addFace(nodes);


	DM::Node c = DM::CGALGeometry::CalculateCentroid(sys, f);

	EXPECT_DOUBLE_EQ(2.0, c.getX());
	EXPECT_DOUBLE_EQ(2.0, c.getZ());

	std::reverse(nodes.begin(), nodes.end());
	DM::Face * f1 = sys->addFace(nodes);
	c = DM::CGALGeometry::CalculateCentroid(sys, f1);
	EXPECT_DOUBLE_EQ(2.0, c.getX());
	EXPECT_DOUBLE_EQ(2.0, c.getZ());

	delete sys;
}

TEST_F(UnitTestsDMExtensions,calculateCentroid3D_YZ){
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);
	DM::System * sys = new DM::System();

	DM::Node * n1 = sys->addNode(DM::Node(0,1,1));
	DM::Node * n2 = sys->addNode(DM::Node(0,3,1));
	DM::Node * n3 = sys->addNode(DM::Node(0,3,3));
	DM::Node * n4 = sys->addNode(DM::Node(0,1,3));

	std::vector<DM::Node * > nodes;
	nodes.push_back(n1);
	nodes.push_back(n2);
	nodes.push_back(n3);
	nodes.push_back(n4);

	DM::Face * f = sys->addFace(nodes);


	DM::Node c = DM::CGALGeometry::CalculateCentroid(sys, f);

	EXPECT_DOUBLE_EQ(2.0, c.getY());
	EXPECT_DOUBLE_EQ(2.0, c.getZ());

	std::reverse(nodes.begin(), nodes.end());
	DM::Face * f1 = sys->addFace(nodes);
	c = DM::CGALGeometry::CalculateCentroid(sys, f1);
	EXPECT_DOUBLE_EQ(2.0, c.getY());
	EXPECT_DOUBLE_EQ(2.0, c.getZ());

	delete sys;
}

TEST_F(UnitTestsDMExtensions, facetoarrangement) {
	Arrangement_2::Edge_iterator					eit;
	Arrangement_2::Face_const_iterator              fit;
	Arrangement_2::Ccb_halfedge_const_circulator    curr;
	Segment_list_2									segments;
	Arrangement_2									arr;

	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);
	DM::System * sys = new DM::System();
	DM::View v("UNIT_TEST", DM::FACE, DM::READ);
	addRectangleWithHole(sys, v);
	mforeach(DM::Component * c, sys->getAllComponentsInView(v)) {
		DM::Face * f = static_cast<DM::Face*> (c);
		DM::CGALGeometry_P::AddFaceToArrangement(arr, f);
	}

	for (fit = arr.faces_begin(); fit != arr.faces_end(); ++fit) {
		if (fit->is_unbounded()) {
			DM::Logger(DM::Debug) << "Unbounded Face";
			continue;
		} else {
			DM::Logger(DM::Debug) << "Outerboundary Face";
		}
		if (fit->has_outer_ccb ()) {
			DM::Logger(DM::Debug) << "Outer CCB";
		}
		if (fit->is_fictitious ()) {
			DM::Logger(DM::Debug) << "Fictitious CCB";

		}
		Arrangement_2::Ccb_halfedge_const_circulator hec = fit->outer_ccb();
		Arrangement_2::Ccb_halfedge_const_circulator end = hec;
		bool isHole = true;
		do{
			++hec;
			if(hec->twin()->face()->is_unbounded()) {
				DM::Logger(DM::Debug) << "Hole Check";
				isHole = false;
			}
		} while(hec != end );
		if (isHole)
			DM::Logger(DM::Debug) << "is hole";
		hec = fit->outer_ccb();
		end = hec;
		do{
			++hec;
			float x = CGAL::to_double(hec->curve().source().x());
			float y = CGAL::to_double(hec->curve().source().y());
			DM::Logger(DM::Debug) << x << "\t" << y;
		}
		while(hec != end );

		Arrangement_2::Hole_const_iterator hi;
		for (hi = fit->holes_begin(); hi != fit->holes_end(); ++hi) {
			DM::Logger(DM::Debug) << "Hole";
		}
	}

	DM::Logger(DM::Debug) << "Faces " << arr.number_of_faces();

}

TEST_F(UnitTestsDMExtensions, edgeToSegment2D) {
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);
	DM::System * sys = new DM::System();
	DM::View v("UNIT_TEST", DM::FACE, DM::READ);
	addRectangleWithHole(sys, v);

	Arrangement_2::Edge_iterator					eit;
	Arrangement_2::Face_const_iterator              fit;
	Arrangement_2::Ccb_halfedge_const_circulator    curr;
	Segment_list_2									segments;
	Arrangement_2									arr;

	segments = DM::CGALGeometry_P::EdgeToSegment2D(sys, v);
	DM::Logger(DM::Debug) << "Segments " << segments.size();
	EXPECT_EQ(8, segments.size());
	DM::Logger(DM::Debug) << "Faces " << arr.number_of_faces();


}


TEST_F(UnitTestsDMExtensions, intersectionTest_with_holes){
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);
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


	DM::Node * n1_h = sys->addNode(DM::Node(1.25,1.25,0));
	DM::Node * n2_h = sys->addNode(DM::Node(1.75,1.25,0));
	DM::Node * n3_h = sys->addNode(DM::Node(1.75,1.75,0));
	DM::Node * n4_h = sys->addNode(DM::Node(1.25,1.75,0));

	std::vector<DM::Node * > nodes_h;
	nodes_h.push_back(n1_h);
	nodes_h.push_back(n2_h);
	nodes_h.push_back(n3_h);
	nodes_h.push_back(n4_h);
	nodes_h.push_back(n1_h);


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
	f1->addHole(nodes_h);
	DM::Face * f2 = sys->addFace(nodes1);


	std::vector<DM::Face *> interested;
	interested    = DM::CGALGeometry::IntersectFace(sys, f1, f2);

	double area_i = TBVectorData::CalculateArea(interested[0]->getNodePointers());
	DM::Logger(DM::Standard) << area_i;
	EXPECT_DOUBLE_EQ(0.1875, area_i);
}

TEST_F(UnitTestsDMExtensions, triangulation){
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);

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
TEST_F(UnitTestsDMExtensions, triangulation_with_holes){
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);

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

TEST_F(UnitTestsDMExtensions, regular_triangulation){
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);

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

TEST_F(UnitTestsDMExtensions, intersectionTest){
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);
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


	std::vector<DM::Face *> interested;
	interested    = DM::CGALGeometry::IntersectFace(sys, f1, f2);

	double area_i = TBVectorData::CalculateArea(interested[0]->getNodePointers());
	EXPECT_DOUBLE_EQ(0.25, area_i);
}

TEST_F(UnitTestsDMExtensions, dointersectionTest_with_hole){
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);
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



	DM::Node * n1_h = sys->addNode(DM::Node(1.25,1.25,0));
	DM::Node * n2_h = sys->addNode(DM::Node(1.75,1.25,0));
	DM::Node * n3_h = sys->addNode(DM::Node(1.75,1.75,0));
	DM::Node * n4_h = sys->addNode(DM::Node(1.25,1.75,0));

	std::vector<DM::Node * > nodes_h;
	nodes_h.push_back(n1_h);
	nodes_h.push_back(n2_h);
	nodes_h.push_back(n3_h);
	nodes_h.push_back(n4_h);

	DM::Face * f1 = sys->addFace(nodes);
	DM::Face * fh = sys->addFace(nodes_h);
	DM::Face * f_with_hole = sys->addFace(nodes);
	f_with_hole->addHole(fh);

	ASSERT_TRUE(DM::CGALGeometry::DoFacesInterect(f1, fh));

	std::vector<DM::Face *> interested;
	interested    = DM::CGALGeometry::IntersectFace(sys, f_with_hole, fh);

	DM::Logger(DM::Standard) << interested.size();

	ASSERT_FALSE(DM::CGALGeometry::DoFacesInterect( fh, f_with_hole));

}

TEST_F(UnitTestsDMExtensions, dointersectionTest){

	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);
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

	DM::Node * n1_1 = sys->addNode(DM::Node(1.5,1.5,0));
	DM::Node * n2_1 = sys->addNode(DM::Node(2.5,1.5,0));
	DM::Node * n3_1 = sys->addNode(DM::Node(2.5,2.5,0));
	DM::Node * n4_1 = sys->addNode(DM::Node(1.5,2.5,0));

	std::vector<DM::Node * > nodes1;
	nodes1.push_back(n1_1);
	nodes1.push_back(n2_1);
	nodes1.push_back(n3_1);
	nodes1.push_back(n4_1);



	DM::Node * n1_2 = sys->addNode(DM::Node(3.5,3.5,0));
	DM::Node * n2_2 = sys->addNode(DM::Node(4.5,3.5,0));
	DM::Node * n3_2 = sys->addNode(DM::Node(4.5,4.5,0));
	DM::Node * n4_2 = sys->addNode(DM::Node(3.5,4.5,0));

	std::vector<DM::Node * > nodes2;
	nodes2.push_back(n1_2);
	nodes2.push_back(n2_2);
	nodes2.push_back(n3_2);
	nodes2.push_back(n4_2);


	ASSERT_TRUE(DM::CGALGeometry::DoFacesInterect(sys->addFace(nodes), sys->addFace(nodes1)));
	ASSERT_FALSE(DM::CGALGeometry::DoFacesInterect(sys->addFace(nodes),sys->addFace(nodes2)));

}

TEST_F(UnitTestsDMExtensions, simpleNodeSearch){
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);

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

TEST_F(UnitTestsDMExtensions, skeletonisation){
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);

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

TEST_F(UnitTestsDMExtensions,calculateArea){
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);
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

	double area = DM::CGALGeometry::CalculateArea2D(f);

	EXPECT_DOUBLE_EQ(1, area);

	delete sys;
}
TEST_F(UnitTestsDMExtensions,calculateAreaWithHole){
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);
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

	double area = DM::CGALGeometry::CalculateArea2D(f);

	EXPECT_DOUBLE_EQ(3, area);

	delete sys;
}

TEST_F(UnitTestsDMExtensions,calculateAreaR9){
	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);
	DM::System * sys = new DM::System();


	DM::Node * n1 = sys->addNode(DM::Node(0,1,0));
	DM::Node * n2 = sys->addNode(DM::Node(0,2,0));
	DM::Node * n3 = sys->addNode(DM::Node(1,2,0));

	DM::Node * n4 = sys->addNode(DM::Node(2,2,0));
	DM::Node * n5 = sys->addNode(DM::Node(2,1,0));
	DM::Node * n6 = sys->addNode(DM::Node(2,0,0));
	DM::Node * n7 = sys->addNode(DM::Node(1,0,0));
	DM::Node * n8 = sys->addNode(DM::Node(0,0,0));

	std::vector<DM::Node * > nodes;
	nodes.push_back(n1);
	nodes.push_back(n2);
	nodes.push_back(n3);
	nodes.push_back(n4);
	nodes.push_back(n5);
	nodes.push_back(n6);
	nodes.push_back(n7);
	nodes.push_back(n8);

	for (int i = 0; i < 8; i++) {
		std::vector<DM::Node*> nodes_test;
		for (int j = 0; j < 8; j++) {
			int pos = i+j;
			if  (pos > 7) pos = pos - 8;
			nodes_test.push_back(nodes[pos]);
		}
		nodes_test.push_back(nodes[i]);

		DM::Face * f = sys->addFace(nodes_test);

		double area = DM::CGALGeometry::CalculateArea2D(f);

		EXPECT_DOUBLE_EQ(4, area);
	}
	delete sys;
}

}
