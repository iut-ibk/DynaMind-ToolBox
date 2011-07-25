file(GLOB VIBE_CPPS *.cpp)

add_executable(vibe2 ${VIBE_CPPS})
target_link_libraries(vibe2 vibe2core ${Boost_LIBRARIES} ${QT_LIBRARIES} ${PYTHON_LIBRARIES})

# Installation of the program
INSTALL(TARGETS vibe2
        DESTINATION "bin"
        PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
