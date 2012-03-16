file(GLOB MODULE_CPPS *.cpp)
file(GLOB MODULE_UI_FILES *.ui)

if (NOT WIN32)
        set(CMAKE_CXX_FLAGS "-frounding-math" )
endif()
set(CMAKE_MODULE_PATH
  #"${vibe2modules_SOURCE_DIR}/CMake"
  #"${vibe2modules_SOURCE_DIR}/CMake/Utils"
  "${CMAKE_CURRENT_SOURCE_DIR}/CMake/Packages"
)

find_package(vibe2core)

include_directories(.)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${VIBE2CORE_INCLUDE_DIRS})
include_directories(../DMToolboxes)

add_library(dynamind-vibe2 SHARED ${MODULE_CPPS})

target_link_libraries(dynamind-vibe2 dynamindtoolbox dynamindcore  ${VIBE2CORE_LIBRARIES} ${VIBE2HELPER_LIBRARIES} ${QT_LIBRARIES})

INSTALL(TARGETS dynamind-vibe2
        DESTINATION "lib"
        PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)


