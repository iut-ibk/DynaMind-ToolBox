CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(DynAlp)
set(CMAKE_MODULE_PATH 
	${CMAKE_MODULE_PATH}
        "${DynAlp_SOURCE_DIR}/cmake"
        "${DynAlp_SOURCE_DIR}/cmake/Utils"
        "${DynAlp_SOURCE_DIR}/cmake/Packages"
)


FIND_PATH(DYNAMINDDEV DynaMindDevCmake.txt
            PATHS   $ENV{DYNAMIND_DIR}/include
            PATH_SUFFIXES dynamindcore/CMake)

IF(DYNAMIND_INCLUDE_DIR)
        MESSAGE(STATUS "Compiled from toolbox")
        MESSAGE(STATUS "DYNAMIND_INCLUDE_DIR ${DYNAMIND_INCLUDE_DIR}")
        SET (DYNAMIND_CORE_LIBRARY dynamindcore)
        SET (DYNAMIND_TOOLBOX_LIBRARY dynamindtoolbox)
        SET (DYNAMIND_EXTENSIONS_LIBRARY dynamind-extensions)
        MESSAGE(STATUS "${DYNAMIND_CORE_LIBRARY}")
ELSE()
        MESSAGE(STATUS "Compiled from stand alone")
        FIND_PATH(DYNAMINDDEV DynaMindDevCmake.txt
        PATHS $ENV{DYNAMIND_DIR}/include
        PATH_SUFFIXES dynamindcore/CMake)

        IF(DYNAMINDDEV)
                INCLUDE(${DYNAMINDDEV}/DynaMindDevCmake.txt)
        ELSE()
                MESSAGE(FATAL_ERROR "Could not find dynamind. Maybe not installed ?")
        ENDIF()

ENDIF()

FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui QtSql REQUIRED)
SET(QT_USE_QTSQL 1)

FIND_PACKAGE(GDAL)
INCLUDE_DIRECTORIES(${GDAL_INCLUDE_DIR})

SET(CGAL_DONT_OVERRIDE_CMAKE_FLAGS "ON" CACHE BOOL "")
find_package(CGAL QUIET COMPONENTS Core REQUIRED)
include(${CGAL_USE_FILE})

INCLUDE(${QT_USE_FILE})
INCLUDE_DIRECTORIES( ${DYNAMIND_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${DYNAMIND_EXTENSIONS_INCLUDE_DIR})


find_package(OpenMP)
if(OPENMP_FOUND)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        message(STATUS "OpenMP is enabled")
else()
        add_definitions(-DOPENMP_DISABLED)
        message(STATUS "OpenMP is disabled. Consider using a compiler that supports OpenMP")
endif()

if (CMAKE_BUILD_TYPE STREQUAL Debug)
        message(STATUS Debug)
        add_definitions(-DDEBUG)
else ()
        message(STATUS Release)
endif()

IF(CMAKE_BUILD_TYPE STREQUAL Debug)
        MESSAGE(STATUS "Build Type = Debug")
        ADD_DEFINITIONS(-DDEBUG)
ELSE()
        MESSAGE(STATUS "Build Type = Release")
ENDIF()
IF(DYNAMIND_TOOLBOX_BUILD_DIR)
    SET(LIBRARY_OUTPUT_PATH ${DYNAMIND_TOOLBOX_BUILD_DIR}/Modules)
    SET(EXECUTABLE_OUTPUT_PATH ${DYNAMIND_TOOLBOX_BUILD_DIR}/Modules)
    SET(SCRIPT_PATH ${DYNAMIND_TOOLBOX_BUILD_DIR}/PythonModules )
ELSE()
    SET(LIBRARY_OUTPUT_PATH ${DynAlp_BINARY_DIR})
    SET(EXECUTABLE_OUTPUT_PATH ${DynAlp_BINARY_DIR})
    SET(SCRIPT_PATH ${DynAlp_BINARY_DIR}/PythonModules )
ENDIF()
# ADD_SUBDIRECTORY(scripts)
ADD_SUBDIRECTORY(src)

