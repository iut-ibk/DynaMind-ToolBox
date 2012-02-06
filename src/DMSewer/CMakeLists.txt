file(GLOB MODULE_CPPS *.cpp)

include_directories(.)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(../DMToolboxes)

add_library(dynamindsewer SHARED ${MODULE_CPPS})

target_link_libraries (dynamindsewer dynamindtoolbox dynamindcore ${QT_LIBRARIES})

INSTALL(TARGETS dynamindsewer
        DESTINATION "lib"
        PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)


