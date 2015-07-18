INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}
                    ${gtest_SOURCE_DIR}/include
                    ${dynamind_SOURCE_DIR}/src/DMTestModules)

FILE(GLOB MEMORY_CPP *.cpp)
ADD_EXECUTABLE(memory-test ${MEMORY_CPP})
TARGET_LINK_LIBRARIES(memory-test dynamindcore dynamind-testmodules)
