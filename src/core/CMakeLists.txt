file(GLOB VIBE_CPPS *.cpp)

add_library(vibecore  SHARED ${VIBE_CPPS})
target_link_libraries(vibecore  ${Boost_LIBRARIES} ${QT_LIBRARIES} ${PYTHON_LIBRARIES})
