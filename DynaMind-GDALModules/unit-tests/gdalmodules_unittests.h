#ifndef GDALMODULES_UNITTESTS_H
#define GDALMODULES_UNITTESTS_H

#include <gtest/gtest.h>
#include <iostream>
#include <QCoreApplication>

namespace {
class GDALModules_Unittests : public ::testing::Test
	{


	};
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);




	return RUN_ALL_TESTS();
}

#endif // GDALMODULES_UNITTESTS_H
