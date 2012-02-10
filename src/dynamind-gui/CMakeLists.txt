
include_directories(../core/)
include_directories(../libdynamind-gui/)
include_directories(.)
MESSAGE(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../libdynamind-gui)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../libdynamind-gui)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/../dynamind-gui/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../dynamind-gui)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

include_directories(muparser/include ../3dparty/qwt-5.2.0/src)
file(GLOB VIBE2 *.cpp)
QT4_ADD_RESOURCES(VIBE2 icons.qrc)

add_executable(dynamind-gui ${VIBE2} )
target_link_libraries(dynamind-gui dynamindgui dynamindcore ${OPENGL_LIBRARIES} ${QT_LIBRARIES} ${QGLVIEWER_LIBRARIES})

INSTALL(TARGETS dynamind-gui
        DESTINATION "bin"
        PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)


