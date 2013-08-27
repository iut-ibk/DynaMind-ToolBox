FILE(GLOB DYNAVIBE_CPPS *.cpp ./ws/*.cpp ./ws/performance/*.cpp ./graph/*.cpp)

INCLUDE_DIRECTORIES(. ./ws ./graph ./ws/performance)

ADD_LIBRARY(dynamind-dynavibe SHARED ${DYNAVIBE_CPPS})

IF(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
	SET_TARGET_PROPERTIES(dynamind-dynavibe  PROPERTIES COMPILE_FLAGS "-frounding-math -fPIC")
ENDIF()

TARGET_LINK_LIBRARIES(dynamind-dynavibe ${DYNAMIND_CORE_LIBRARY} ${Boost_LIBRARIES} ${DYNAMIND_TOOLBOX_LIBRARY} ${QT_LIBRARIES} ${CGAL_LIBRARIES} epanet2)
