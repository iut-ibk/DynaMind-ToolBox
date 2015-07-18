SET(gtest_force_shared_crt "ON" CACHE BOOL "")
ADD_SUBDIRECTORY(gtest-1.7.0)
MESSAGE(STATUS "gtest_force_shared_crt = ${gtest_force_shared_crt}")
enable_testing()

FILE(GLOB DYNAMIND_CPPS *.cpp)
FILE(GLOB DYNAMIND_HEADER *.h)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}
                    ${gtest_SOURCE_DIR}/include
                    ${dynamind_SOURCE_DIR}/src/DMTestModules
                    ${dynamind_SOURCE_DIR}/src/DMToolboxes)

FILE(GLOB GTEST_SOURCE_FILES *.cpp)


ADD_EXECUTABLE(unit-test ${GTEST_SOURCE_FILES})

IF(USEQT5)
    TARGET_LINK_LIBRARIES(unit-test gtest_main dynamindcore dynamindtoolbox dynamind-testmodules Qt5::Gui Qt5::Core Qt5::Sql Qt5::Concurrent Qt5::Xml ${GDAL_LIBRARY})
ELSE()
    TARGET_LINK_LIBRARIES(unit-test gtest_main dynamindcore dynamindtoolbox dynamind-testmodules ${QT_LIBRARIES} ${QT_QTMAIN_LIBRARY} ${QT_QTSQL_LIBRARIES} ${GDAL_LIBRARY})
ENDIF()

set(TESTS
	TestGDALModules.GDAL_ADVANCE_API_TEST,
	TestGDALModules.TestInsertSpeed_DM,
	TestGDALModules.TestInsertSpeed,
	TestGDALModules.ConnectionTest,
	TestGDALModules.BranchingTest,
	TestGDALModules.UpdateTest,
	TestGDALModules.BranchModify,
	TestGDALPython.LoadPython,
	TestGDALPython.PythonConnectionTest,
	TestGDALPython.PythonReadTest,
	TestGDALPython.PythonModifyTest
)

foreach(utest ${TESTS})
ADD_TEST(NAME ${utest}
	 WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	 COMMAND unit-test --gtest_filter=${utest})
endforeach()




