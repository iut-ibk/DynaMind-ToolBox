INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})


SET( TestSimulation_SRCS testsimulation.cpp )
QT4_AUTOMOC( ${TestSimulation_SRCS} )
ADD_EXECUTABLE( TestSimulation ${TestSimulation_SRCS} )
TARGET_LINK_LIBRARIES( TestSimulation dynamindcore ${QT_QTTEST_LIBRARY} ${QT_LIBRARIES} ${QT_QTMAIN_LIBRARY} )
ADD_TEST( NAME TestSimulation COMMAND TestSimulation )
