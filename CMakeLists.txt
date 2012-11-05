cmake_minimum_required(VERSION 2.8)
project(DynaMind-ToolBox)

ADD_DEFINITIONS(-DDYNAMIND_VERSION_CORE="0.3.0")

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
OPTION(WITH_UNIT_TESTS "build unit tests" OFF)
OPTION(WITH_PLUGIN_DYNAMIND "if with DYNAMIND plugin" OFF)
OPTION(WITH_PLUGIN_PERFORMANCE_ASSESSMENT "if with performance assessment plugin" OFF)
OPTION(WITH_PLUGIN_DYNAVIBE "if with DynaVIBe plugin" OFF)
OPTION(WITH_PLUGIN_POSTGIS "if with POSTGIS plugin" OFF)
OPTION(WITH_PLUGIN_POWERVIBE "if with PowerVIBe plugin" OFF)
OPTION(WITH_PLUGIN_URBANSIM "if with UrbanSim plugin" OFF)
OPTION(WITH_PLUGIN_SEWER "if with Sewer plugin" OFF)
OPTION(WITH_PLUGIN_DANCE4WATER "if with DAnCE4Water plugin" OFF)
OPTION(WITH_PLUGIN_VIBE "if with VIBeModules plugin" OFF)
OPTION(WITH_PLUGIN_CITYBLOCKS "if with Cityblocks plugin" OFF)
OPTION(WITH_EXTENSIONS "if with Extensions plugin" ON)

find_package(Git REQUIRED)

set(TOOLBOX DynaMind DynaMind-Gui DynaMind-BasicModules)

if(WITH_UNIT_TESTS)
    set(WITH_PLUGIN_CITYBLOCKS ON CACHE BOOL "" FORCE)
    set(WITH_PLUGIN_SEWER ON CACHE BOOL "" FORCE)
endif(WITH_UNIT_TESTS)

if(WITH_EXTENSIONS)
    set(TOOLBOX ${TOOLBOX} DynaMind-Extensions)
endif(WITH_EXTENSIONS)

if(WITH_PLUGIN_DYNAMIND)
    set(TOOLBOX ${TOOLBOX} DynaMind-DYNAMIND)
endif(WITH_PLUGIN_DYNAMIND)

if(WITH_PLUGIN_PERFORMANCE_ASSESSMENT)
    set(TOOLBOX ${TOOLBOX} DynaMind-Performance-Assessment)
endif(WITH_PLUGIN_PERFORMANCE_ASSESSMENT)

if(WITH_PLUGIN_DYNAVIBE)
    set(TOOLBOX ${TOOLBOX} DynaVIBe)
endif(WITH_PLUGIN_DYNAVIBE)

if(WITH_PLUGIN_POSTGIS)
    set(TOOLBOX ${TOOLBOX} DynaMind-PostGIS)
endif(WITH_PLUGIN_POSTGIS)

if(WITH_PLUGIN_POWERVIBE)
    set(TOOLBOX ${TOOLBOX} PowerVIBe)
endif(WITH_PLUGIN_POWERVIBE)

if(WITH_PLUGIN_URBANSIM)
    set(TOOLBOX ${TOOLBOX} DynaMind-UrbanSim)
endif(WITH_PLUGIN_URBANSIM)

if(WITH_PLUGIN_SEWER)
    set(TOOLBOX ${TOOLBOX} DynaMind-Sewer)
endif(WITH_PLUGIN_SEWER)

if(WITH_PLUGIN_DANCE4WATER)
    set(TOOLBOX ${TOOLBOX} DynaMind-DAnCE4Water)
endif(WITH_PLUGIN_DANCE4WATER)

if(WITH_PLUGIN_VIBE)
    set(TOOLBOX ${TOOLBOX} DynaMind-VIBeModules)
endif(WITH_PLUGIN_VIBE)

if(WITH_PLUGIN_CITYBLOCKS)
    set(TOOLBOX ${TOOLBOX} DynaMind-CityBlocks)
endif(WITH_PLUGIN_CITYBLOCKS)

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
            execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --recursive ${tool}
                            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
    endif(DYNAMIND_UPDATE)
endforeach(tool ${TOOLBOX}) 

SET(DYNAMIND_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/DynaMind/src/core) 
SET(DYNAMIND_INCLUDE_DIR ${DYNAMIND_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/DynaMind/src/DMToolboxes) 
SET(DYNAMIND_INCLUDE_DIR ${DYNAMIND_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/DynaMind/src/python-swig) 
SET(DYNAMIND_EXTENSIONS_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/DynaMind-Extensions/src) 
SET(DYNAMIND_TOOLBOX_BUILD_DIR ${DynaMind-ToolBox_BINARY_DIR}/output)


foreach (tool ${TOOLBOX})
    add_subdirectory(${tool})
endforeach(${tool}) 

if(WITH_UNIT_TESTS)
    add_subdirectory(DynaMind-ComplexUnitTests)
endif(WITH_UNIT_TESTS)

add_subdirectory(Data)

if(WITH_PYTHON_PACKAGE)
    INCLUDE(DynaMind-ToolBoxCPack_with_Python.txt)
else()
    INCLUDE(DynaMind-ToolBoxCPack.txt)
endif()
