CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(dynamind)

SET(CMAKE_MODULE_PATH
  "${dynamind_SOURCE_DIR}/CMake"
  "${dynamind_SOURCE_DIR}/CMake/Utils" 
  "${dynamind_SOURCE_DIR}/CMake/Packages"
)

SET(Boost_USE_STATIC_LIBS   OFF)
FIND_PACKAGE(Boost 1.38.0 COMPONENTS program_options date_time REQUIRED)
FIND_PACKAGE(PythonLibs REQUIRED)
FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui)

#find_package(CGAL QUIET COMPONENTS Core )

SET(QT_USE_QTXML 1)
INCLUDE(${QT_USE_FILE})
#include(${CGAL_USE_FILE})

INCLUDE_DIRECTORIES(src/core src/python-swig ${Boost_INCLUDE_DIRS} ${QT_QTCORE_INCLUDE_DIR} ${PYTHON_INCLUDE_PATH})
FIND_PACKAGE(SWIG REQUIRED)
INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})


IF(CMAKE_BUILD_TYPE STREQUAL Debug)
        MESSAGE(STATUS "Build Type = Debug")
        ADD_DEFINITIONS(-DDEBUG)
ELSE()
        MESSAGE(STATUS "Build Type = Release")
ENDIF()

SET(LIBRARY_OUTPUT_PATH ${dynamind_BINARY_DIR})
SET(EXECUTABLE_OUTPUT_PATH ${dynamind_BINARY_DIR})


ADD_SUBDIRECTORY(src/core)
ADD_SUBDIRECTORY(src/python-swig)
ADD_SUBDIRECTORY(src/DMTest)
ADD_SUBDIRECTORY(src/DMToolboxes)


#ADD_SUBDIRECTORY(src/libdynamind-gui)
#ADD_SUBDIRECTORY(src/dynamind-gui)
#ADD_SUBDIRECTORY(src/DMTestModules)
#ADD_SUBDIRECTORY(src/console)
#ADD_SUBDIRECTORY(src/libdynamind-gui)
#ADD_SUBDIRECTORY(src/dynamind-gui)
#ADD_SUBDIRECTORY(src/CityBlocks)
#ADD_SUBDIRECTORY(src/DMSewer)
#ADD_SUBDIRECTORY(src/DMExampleSimulations)
#ADD_SUBDIRECTORY(src/UDMSitzenfrei)

#at the moment just for linux
#if (NOT WIN32)
#        ADD_SUBDIRECTORY(src/dynamind-vibe2)
#endif()

#ADD_SUBDIRECTORY(src/qgis-plugin)


#INCLUDE(DynaMindCPack.txt)
