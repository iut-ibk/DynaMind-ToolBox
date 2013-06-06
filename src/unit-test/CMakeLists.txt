SET(gtest_force_shared_crt "ON" CACHE BOOL "")
ADD_SUBDIRECTORY(gtest-1.6.0)
MESSAGE(STATUS "gtest_force_shared_crt = ${gtest_force_shared_crt}")

# sql stuff
FIND_PACKAGE(Qt4 REQUIRED)
SET(QT_USE_QTSQL true)
# ADD_DEFINITIONS(-DQT_NO_KEYWORDS)
INCLUDE(${QT_USE_FILE})

find_package(OpenMP)
if(OPENMP_FOUND)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        message(STATUS "OpenMP is enabled")
else()
        add_definitions(-DOPENMP_DISABLED)
        message(STATUS "OpenMP is disabled. Consider using a compiler that supports OpenMP")
endif()

FILE(GLOB DYNAMIND_CPPS *.cpp)
FILE(GLOB DYNAMIND_HEADER *.h)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}
                    ${gtest_SOURCE_DIR}/include
                    ${dynamind_SOURCE_DIR}/src/DMTestModules
                    ${dynamind_SOURCE_DIR}/src/DMToolboxes)

FILE(GLOB UNIT_CPP *.cpp)


ADD_EXECUTABLE(unit-test ${UNIT_CPP})

TARGET_LINK_LIBRARIES(unit-test gtest_main dynamindcore dynamindtoolbox dynamind-testmodules
    ${QT_LIBRARIES} ${QT_QTMAIN_LIBRARY} ${QT_QTSQL_LIBRARIES})

