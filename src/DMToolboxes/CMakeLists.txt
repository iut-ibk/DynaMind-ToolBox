file(GLOB MODULE_CPPS *.cpp)

include_directories(.)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(dynamindtoolbox SHARED ${MODULE_CPPS})

target_link_libraries(dynamindtoolbox dynamindcore ${QT_LIBRARIES})

INSTALL(TARGETS dynamindtoolbox
        DESTINATION "lib"
        PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)


