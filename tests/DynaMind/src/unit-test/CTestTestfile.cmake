# CMake generated Testfile for 
# Source directory: /workspaces/DynaMind-ToolBox/DynaMind/src/unit-test
# Build directory: /workspaces/DynaMind-ToolBox/tests/DynaMind/src/unit-test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(TestGDALModules.GDAL_ADVANCE_API_TEST, "/workspaces/DynaMind-ToolBox/tests/output/unit-test" "--gtest_filter=TestGDALModules.GDAL_ADVANCE_API_TEST,")
set_tests_properties(TestGDALModules.GDAL_ADVANCE_API_TEST, PROPERTIES  WORKING_DIRECTORY "/workspaces/DynaMind-ToolBox/tests")
add_test(TestGDALModules.TestInsertSpeed_DM, "/workspaces/DynaMind-ToolBox/tests/output/unit-test" "--gtest_filter=TestGDALModules.TestInsertSpeed_DM,")
set_tests_properties(TestGDALModules.TestInsertSpeed_DM, PROPERTIES  WORKING_DIRECTORY "/workspaces/DynaMind-ToolBox/tests")
add_test(TestGDALModules.TestInsertSpeed, "/workspaces/DynaMind-ToolBox/tests/output/unit-test" "--gtest_filter=TestGDALModules.TestInsertSpeed,")
set_tests_properties(TestGDALModules.TestInsertSpeed, PROPERTIES  WORKING_DIRECTORY "/workspaces/DynaMind-ToolBox/tests")
add_test(TestGDALModules.ConnectionTest, "/workspaces/DynaMind-ToolBox/tests/output/unit-test" "--gtest_filter=TestGDALModules.ConnectionTest,")
set_tests_properties(TestGDALModules.ConnectionTest, PROPERTIES  WORKING_DIRECTORY "/workspaces/DynaMind-ToolBox/tests")
add_test(TestGDALModules.BranchingTest, "/workspaces/DynaMind-ToolBox/tests/output/unit-test" "--gtest_filter=TestGDALModules.BranchingTest,")
set_tests_properties(TestGDALModules.BranchingTest, PROPERTIES  WORKING_DIRECTORY "/workspaces/DynaMind-ToolBox/tests")
add_test(TestGDALModules.UpdateTest, "/workspaces/DynaMind-ToolBox/tests/output/unit-test" "--gtest_filter=TestGDALModules.UpdateTest,")
set_tests_properties(TestGDALModules.UpdateTest, PROPERTIES  WORKING_DIRECTORY "/workspaces/DynaMind-ToolBox/tests")
add_test(TestGDALModules.BranchModify, "/workspaces/DynaMind-ToolBox/tests/output/unit-test" "--gtest_filter=TestGDALModules.BranchModify,")
set_tests_properties(TestGDALModules.BranchModify, PROPERTIES  WORKING_DIRECTORY "/workspaces/DynaMind-ToolBox/tests")
add_test(TestGDALPython.LoadPython, "/workspaces/DynaMind-ToolBox/tests/output/unit-test" "--gtest_filter=TestGDALPython.LoadPython,")
set_tests_properties(TestGDALPython.LoadPython, PROPERTIES  WORKING_DIRECTORY "/workspaces/DynaMind-ToolBox/tests")
add_test(TestGDALPython.PythonConnectionTest, "/workspaces/DynaMind-ToolBox/tests/output/unit-test" "--gtest_filter=TestGDALPython.PythonConnectionTest,")
set_tests_properties(TestGDALPython.PythonConnectionTest, PROPERTIES  WORKING_DIRECTORY "/workspaces/DynaMind-ToolBox/tests")
add_test(TestGDALPython.PythonReadTest, "/workspaces/DynaMind-ToolBox/tests/output/unit-test" "--gtest_filter=TestGDALPython.PythonReadTest,")
set_tests_properties(TestGDALPython.PythonReadTest, PROPERTIES  WORKING_DIRECTORY "/workspaces/DynaMind-ToolBox/tests")
add_test(TestGDALPython.PythonModifyTest "/workspaces/DynaMind-ToolBox/tests/output/unit-test" "--gtest_filter=TestGDALPython.PythonModifyTest")
set_tests_properties(TestGDALPython.PythonModifyTest PROPERTIES  WORKING_DIRECTORY "/workspaces/DynaMind-ToolBox/tests")
subdirs("gtest-1.7.0")
