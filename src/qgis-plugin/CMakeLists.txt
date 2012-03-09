file(GLOB MODULE_CPPS *.cpp)

include_directories(.)
include_directories(../DMToolboxes)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(/home/c8451045/Documents/Quantum-GIS/src/plugins)
add_library(qgis-plugin SHARED ${MODULE_CPPS})

target_link_libraries(qgis-plugin dynamindtoolbox dynamindcore ${QT_LIBRARIES})

INSTALL(TARGETS qgis-plugin
        DESTINATION "lib"
        PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)


