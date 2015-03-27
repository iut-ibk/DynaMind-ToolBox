cmake_minimum_required(VERSION 2.8)
project(DynaMind-ToolBox)

IF(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
	message(STATUS "Using Visual C compilers")
	set(CMAKE_CXX_FLAGS "/EHsc /bigobj")
	ADD_DEFINITIONS(-D_VARIADIC_MAX=9 -DCGAL_CFG_NO_CPP0X_ISFINITE)
ENDIF()

OPTION(DYNAMIND_UPDATE "update before compile" OFF)
OPTION(WITH_INSTALLER "create a windows installer (NSIS)" OFF)
OPTION(WITH_UNIT_TESTS "build unit tests" OFF)
OPTION(WITH_PLUGIN_DYNALP "if with DYNALP plugin" OFF)
OPTION(WITH_PLUGIN_DYNAVIBE "if with DynaVIBe plugin" OFF)
OPTION(WITH_PLUGIN_SEWER "if with Sewer plugin" ON)
OPTION(WITH_PLUGIN_BASICMODULES "if with BASICMODULES plugin" ON)
OPTION(WITH_PLUGIN_GDALMODULE "if with GDALMODULES plugin" OFF)
OPTION(WITH_STANDALONE_EXE "if with standalon executeable" ON)
OPTION(WITH_GUI "if with GUI" ON)
OPTION(WITH_EXTENSIONS "if with Extensions plugin" ON)
OPTION(WITH_PLUGIN_PERFORMANCE_ASSESSMENT "if with performance assessment plugin" OFF)
OPTION(WITH_DOC "Create Docu" OFF)
OPTION(PYTHON_EMBEDDING "Embedding Python" OFF)
OPTION(USEQT5 "Use Qt5" OFF)

find_package(Git REQUIRED)

set(TOOLBOX DynaMind)


if(NOT USEQT5 AND WITH_PLUGIN_BASICMODULES)
    set(TOOLBOX ${TOOLBOX} DynaMind-BasicModules)
endif(NOT USEQT5 AND WITH_PLUGIN_BASICMODULES)

if(WITH_GUI)
    set(TOOLBOX ${TOOLBOX} DynaMind-Gui)
endif(WITH_GUI)

if(WITH_EXTENSIONS)
    set(TOOLBOX ${TOOLBOX} DynaMind-Extensions)
endif(WITH_EXTENSIONS)

if(NOT USEQT5 AND WITH_PLUGIN_DYNAVIBE)
    set(TOOLBOX ${TOOLBOX} DynaVIBe)
endif(NOT USEQT5 AND WITH_PLUGIN_DYNAVIBE)

if(NOT USEQT5 AND WITH_PLUGIN_DYNALP)
    set(TOOLBOX ${TOOLBOX} DynaMind-DynAlp)
endif(NOT USEQT5 AND WITH_PLUGIN_DYNALP)

if(NOT USEQT5 AND WITH_PLUGIN_SEWER)
    set(TOOLBOX ${TOOLBOX} DynaMind-Sewer)
endif(NOT USEQT5 AND WITH_PLUGIN_SEWER)

if(NOT USEQT5 AND WITH_PLUGIN_GDALMODULE)
    set(TOOLBOX ${TOOLBOX} DynaMind-GDALModules)
endif(NOT USEQT5 AND WITH_PLUGIN_GDALMODULE)

if(NOT USEQT5 AND WITH_PLUGIN_PERFORMANCE_ASSESSMENT)
    set(TOOLBOX ${TOOLBOX} DynaMind-Performance-Assessment)
endif(NOT USEQT5 AND WITH_PLUGIN_PERFORMANCE_ASSESSMENT)

#if(WITH_PLUGIN_POWERVIBE)
#    set(TOOLBOX ${TOOLBOX} PowerVIBe)
#endif(WITH_PLUGIN_POWERVIBE)

#if(WITH_PLUGIN_URBANSIM)
#    set(TOOLBOX ${TOOLBOX} DynaMind-UrbanSim)
#endif(WITH_PLUGIN_URBANSIM)

#if(WITH_PLUGIN_DANCE4WATER)
#    set(TOOLBOX ${TOOLBOX} DynaMind-DAnCE4Water)
#endif(WITH_PLUGIN_DANCE4WATER)

#if(WITH_PLUGIN_VIBE)
#    set(TOOLBOX ${TOOLBOX} DynaMind-VIBeModules)
#endif(WITH_PLUGIN_VIBE)

#if(WITH_PLUGIN_CITYBLOCKS)
#    set(TOOLBOX ${TOOLBOX} DynaMind-CityBlocks)
#endif(WITH_PLUGIN_CITYBLOCKS)

#if(WITH_PLUGIN_QGIS)
#    set(TOOLBOX ${TOOLBOX} DynaMind-QGIS)
#endif(WITH_PLUGIN_QGIS)

if(NOT USEQT5 AND WITH_STANDALONE_EXE)
    set(TOOLBOX ${TOOLBOX} DynaMind-Executable)
endif(NOT USEQT5 AND WITH_STANDALONE_EXE)

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

add_subdirectory(Data)

if(WITH_DOC)
    add_subdirectory(./doc/)
endif()

if(NOT USEQT5 AND WITH_INSTALLER)
    INCLUDE(DynaMind-ToolBoxCPack_with_Python.txt)
endif()
