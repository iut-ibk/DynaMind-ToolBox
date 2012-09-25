cmake_minimum_required(VERSION 2.8)
project(DynaMind-ToolBox)

ADD_DEFINITIONS(-DDYNAMIND_VERSION_CORE="0.2.1")

IF(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
        message(STATUS "Using Visual C compilers")
        set(CMAKE_CXX_FLAGS "/EHsc")
ENDIF()

IF(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
        message(STATUS "Using GNU compilers")
        set(CMAKE_CXX_FLAGS "-g -Wall")
ENDIF()

OPTION(DYNAMIND_UPDATE "update before compile" OFF)
OPTION(WITH_PYTHON_PACKAGE "build python package" ON)
OPTION(WITH_TESTS "build unit tests" OFF)
OPTION(WITH_PLUGIN_QGIS "if with qgis plugin" OFF)
OPTION(WITH_PLUGIN_PERFORMANCE_ASSESSMENT "if with performance assessment plugin" OFF)

find_package(Git REQUIRED)

set(TOOLBOX DynaMind DynaMind-Gui DynaMind-BasicModules DynaMind-Sewer DynaMind-CityBlocks DynaMind-UrbanSim  DynaMind-VIBeModules DynaMind-DAnCE4Water)

if(WITH_PLUGIN_QGIS)
    set(TOOLBOX ${TOOLBOX} DynaMind-QGIS)
endif(WITH_PLUGIN_QGIS)

if(WITH_PLUGIN_PERFORMANCE_ASSESSMENT)
    set(TOOLBOX ${TOOLBOX} DynaMind-Performance-Assessment)
endif(WITH_PLUGIN_PERFORMANCE_ASSESSMENT)

file(MAKE_DIRECTORY ${DynaMind-ToolBox_BINARY_DIR}/output)


foreach (tool ${TOOLBOX})
    if(EXISTS ${CMAKE_SOURCE_DIR}/${tool}/src)
        message(STATUS "${tool} submodule already exists")
    else (EXISTS ${CMAKE_SOURCE_DIR}/${tool}/src)
        message(STATUS "preparing ${tool} submodule for you")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive ${tool}
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    endif (EXISTS ${CMAKE_SOURCE_DIR}/${tool}/src)

    if(DYNAMIND_UPDATE)
            execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive ${tool}
                            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    endif(DYNAMIND_UPDATE)
endforeach(tool ${TOOLBOX}) 

SET(DYNAMIND_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/DynaMind/src/core) 
SET(DYNAMIND_INCLUDE_DIR ${DYNAMIND_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/DynaMind/src/DMToolboxes) 
SET(DYNAMIND_INCLUDE_DIR ${DYNAMIND_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/DynaMind/src/python-swig) 
SET(DYNAMIND_TOOLBOX_BUILD_DIR ${DynaMind-ToolBox_BINARY_DIR}/output)

foreach (tool ${TOOLBOX})
    add_subdirectory(${tool})
endforeach(${tool}) 

if (WITH_UNIT_TESTS)
    add_subdirectory(DynaMind-ComplexUnitTests)
endif (WITH_UNIT_TESTS)
add_subdirectory(Data)
if (WITH_PYTHON_PACKAGE)
    INCLUDE(DynaMind-ToolBoxCPack_with_Python.txt)
else()
    INCLUDE(DynaMind-ToolBoxCPack.txt)
endif()
