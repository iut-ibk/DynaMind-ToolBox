INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_BINARY_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB VIBE_CPPS *.cpp)

SET_SOURCE_FILES_PROPERTIES(${CMAKE_BINARY_DIR}/pydynamindWRAP.cpp PROPERTIES GENERATED 1)
SET(VIBE_CPPS ${VIBE_CPPS} ../python-swig/dmpythonenv.cpp ${CMAKE_BINARY_DIR}/pydynamindWRAP.cpp)

add_executable(exampleSimulations ${VIBE_CPPS})

ADD_DEPENDENCIES(exampleSimulations dynamindcore pydynamindgen)

target_link_libraries(exampleSimulations dynamindcore ${Boost_LIBRARIES} ${QT_LIBRARIES})

# Installation of the program
INSTALL(TARGETS exampleSimulations
        DESTINATION "bin"
        PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
