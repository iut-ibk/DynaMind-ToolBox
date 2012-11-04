SET(gtest_force_shared_crt "ON" CACHE BOOL "")
MESSAGE(STATUS "gtest_force_shared_crt = ${gtest_force_shared_crt}")

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}
                    ${gtest_SOURCE_DIR}/include)

FILE(GLOB UNIT_CPP *.cpp)
ADD_EXECUTABLE(dynamind-complextests ${UNIT_CPP})
TARGET_LINK_LIBRARIES(dynamind-complextests gtest_main  ${DYNAMIND_TOOLBOX_LIBRARY} ${DYNAMIND_CORE_LIBRARY} ${DYNAMIND_EXTENSIONS_LIBRARY} )
