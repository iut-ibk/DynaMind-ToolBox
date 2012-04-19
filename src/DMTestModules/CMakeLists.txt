file(GLOB MODULE_CPPS *.cpp)

include_directories(.)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(dynamind-testmodules SHARED ${MODULE_CPPS})

target_link_libraries (dynamind-testmodules ${DYNAMIND_TOOLBOX_LIBRARY}  ${DYNAMIND_CORE_LIBRARY} ${QT_LIBRARIES})

INSTALL(TARGETS dynamind-testmodules
        DESTINATION "lib"
        PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)


