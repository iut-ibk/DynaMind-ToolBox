CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(dynamind-basicmodules)

FIND_PATH(DYNAMINDDEV DynaMindDevCmake.txt
            PATHS   $ENV{DYNAMIND_DIR}/include
            PATH_SUFFIXES dynamindcore/CMake)

IF(DYNAMINDDEV)
    INCLUDE(${DYNAMINDDEV}/DynaMindDevCmake.txt)
ELSE()
    MESSAGE(FATAL_ERROR "Could not find dynamind. Maybe not installed ?")
ENDIF()

FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui REQUIRED)


INCLUDE(${QT_USE_FILE})
INCLUDE_DIRECTORIES( ${DYNAMIND_INCLUDE_DIR})

IF(CMAKE_BUILD_TYPE STREQUAL Debug)
        MESSAGE(STATUS "Build Type = Debug")
        ADD_DEFINITIONS(-DDEBUG)
ELSE()
        MESSAGE(STATUS "Build Type = Release")
ENDIF()

SET(LIBRARY_OUTPUT_PATH ${dynamind-basicmodules_BINARY_DIR})
SET(EXECUTABLE_OUTPUT_PATH ${dynamind-basicmodules_BINARY_DIR})

ADD_SUBDIRECTORY(scripts)
ADD_SUBDIRECTORY(src/DynaMind-BasicModules)
