file(GLOB VIBE_CPPS *.cpp)

add_executable(vibe ${VIBE_CPPS})
target_link_libraries(vibe vibecore ${Boost_LIBRARIES} ${QT_LIBRARIES} ${PYTHON_LIBRARIES})
