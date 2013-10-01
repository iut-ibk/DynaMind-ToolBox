#include <dmlogsink.h>

#include "unittestsbasicmodules.h"
#include "attributecalculator.h"
#include "offsetface.h"
#include "cgalgeometry.h"

#include <QString>

DM::Face * addRectangle(DM::System* sys, DM::View v)
{
	DM::Node * n1 = sys->addNode(DM::Node(0,0,0));
	DM::Node * n2 = sys->addNode(DM::Node(1,0,0));
	DM::Node * n3 = sys->addNode(DM::Node(1,1,0));
	DM::Node * n4 = sys->addNode(DM::Node(0,1,0));

	std::vector<DM::Node * > nodes;
	nodes.push_back(n1);
	nodes.push_back(n2);
	nodes.push_back(n3);
	nodes.push_back(n4);

	DM::Face * f1 = sys->addFace(nodes, v);

	return f1;
}

/** Unit test for https://github.com/iut-ibk/DynaMind-ToolBox/issues/221
 */
TEST_F(UnitTestsBasicModules, AttributeCalculatorTranslate) {
	AttributeCalculator attrc;
	std::string exp = attrc.IfElseConverter("if(a==1,0,if(b==2,1,3))").toStdString();
	if (!(exp == "(a==1?0:(b==2?1:3))"))
		std::cout << exp << std::endl;

	EXPECT_TRUE(exp == "(a==1?0:(b==2?1:3))");
}

/** Unit offsetFace
 */
TEST_F(UnitTestsBasicModules, OffsetFace) {
	OffsetFace of;

	ostream *out = &cout;
	DM::Log::init(new DM::OStreamLogSink(*out), DM::Standard);

	DM::System * sys = new DM::System();
	DM::View test_in("TEST_IN", DM::FACE, DM::WRITE);

	DM::Face * f = addRectangle(sys, test_in);
	DM::Face * f_out = of.createOffest(sys, f, 0.2);
	EXPECT_DOUBLE_EQ(DM::CGALGeometry::CalculateArea2D(f_out) , 0.36);

	f_out = of.createOffest(sys, f, -0.2);
	EXPECT_DOUBLE_EQ(DM::CGALGeometry::CalculateArea2D(f_out) , 1.96);

	f_out = of.createOffest(sys, f, 0.5);
	EXPECT_TRUE(f_out == NULL);

	f_out = of.createOffest(sys, f, 0.6);
	EXPECT_TRUE(f_out == NULL);
}


