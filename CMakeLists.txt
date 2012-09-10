cmake_minimum_required(VERSION 2.8)
project(DynaMind-ToolBox)

ADD_DEFINITIONS(-DDYNAMIND_VERSION_CORE="0.2.0")

OPTION(DYNAMIND_UPDATE "update before compile" OFF)
OPTION(WITH_PYTHON_PACKAGE "build python package" ON)
OPTION(WITH_TESTS "build unit tests" OFF)

find_package(Git REQUIRED)

set(TOOLBOX DynaMind DynaMind-Gui DynaMind-BasicModules DynaMind-Sewer DynaMind-CityBlocks DynaMind-UrbanSim DynaMind-Performance-Assessment DynaMind-VIBeModules DynaMind-DAnCE4Water)
file(MAKE_DIRECTORY ${DynaMind-ToolBox_BINARY_DIR}/output)


foreach (tool ${TOOLBOX})
    if(EXISTS ${CMAKE_SOURCE_DIR}/${tool}/src)
        message(STATUS "${tool} submodule already exists")
    else (EXISTS ${CMAKE_SOURCE_DIR}/${tool}/src)
        message(STATUS "preparing ${tool} submodule for you")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule init ${tool}
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --recursive ${tool}
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
