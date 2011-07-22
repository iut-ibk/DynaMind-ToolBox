file(GLOB VIBE_CPPS *.cpp)

add_library(vibecore  SHARED ${VIBE_CPPS})
target_link_libraries(vibecore  ${Boost_LIBRARIES} ${QT_LIBRARIES} ${PYTHON_LIBRARIES})

# Installation of the program
INSTALL(TARGETS vibecore
        DESTINATION "lib"
        PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
