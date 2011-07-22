file(GLOB VIBE_CPPS *.cpp)

add_executable(vibe ${VIBE_CPPS})
target_link_libraries(vibe vibecore ${Boost_LIBRARIES} ${QT_LIBRARIES} ${PYTHON_LIBRARIES})

# Installation of the program
INSTALL(TARGETS vibe
        DESTINATION "bin"
        PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
