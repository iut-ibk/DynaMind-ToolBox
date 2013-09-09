#include "unittestsbasicmodules.h"
#include "attributecalculator.h"

#include <QString>

/** Unit test for https://github.com/iut-ibk/DynaMind-ToolBox/issues/221
 */
TEST_F(UnitTestsBasicModules, AttributeCalculatorTranslate) {
	AttributeCalculator attrc;
	std::string exp = attrc.IfElseConverter("if(a==1,0,if(b==2,1,3))").toStdString();
	if (!(exp == "(a==1?0:(b==2?1:3))"))
		std::cout << exp << std::endl;

	EXPECT_TRUE(exp == "(a==1?0:(b==2?1:3))");


}

