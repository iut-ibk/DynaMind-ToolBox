
SET(gtest_force_shared_crt "ON" CACHE BOOL "")
ADD_SUBDIRECTORY(gtest-1.6.0)
MESSAGE(STATUS "gtest_force_shared_crt = ${gtest_force_shared_crt}")
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${gtest_SOURCE_DIR}/include)
FILE(GLOB UNIT_CPP *.cpp)
ADD_EXECUTABLE(unit-test ${UNIT_CPP})
TARGET_LINK_LIBRARIES(unit-test gtest_main dynamindcore)
