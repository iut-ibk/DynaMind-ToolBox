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
   QCoreApplication app(argc, argv);

	RUN_ALL_TESTS();
   return app.exec();
}

#endif // GDALMODULES_UNITTESTS_H
