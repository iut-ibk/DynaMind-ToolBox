SET(gtest_force_shared_crt "ON" CACHE BOOL "")
MESSAGE(STATUS "gtest_force_shared_crt = ${gtest_force_shared_crt}")
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}
		    ${gtest_SOURCE_DIR}/include
		    ${CMAKE_CURRENT_SOURCE_DIR}/../src)

FILE(GLOB UNIT_CPP *.cpp)
ADD_EXECUTABLE(unit-tests-dmextensions ${UNIT_CPP})

TARGET_LINK_LIBRARIES(unit-tests-dmextensions gtest_main  dynamind-extensions ${DYNAMIND_TOOLBOX_LIBRARY} ${DYNAMIND_CORE_LIBRARY} ${QT_LIBRARIES} ${CGAL_LIBRARIES} ${CGAL_3RD_PARTY_LIBRARIES} ${Boost_LIBRARIES})
