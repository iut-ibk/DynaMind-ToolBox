file(GLOB MODULE_CPPS *.cpp)
file(GLOB MODULE_UI_FILES *.ui)
file(GLOB MODULE_CPPS *.cpp ../3dparty/muparser/src/*.cpp)
set(MOC_MODULE_FILES guidynamicinout.h guigrouptest.h guicellularautomata.h guicellularautomata_neighbourhood.h guiequationeditor.h)

QT4_WRAP_UI(MODULE_CPPS ${MODULE_UI_FILES})
QT4_WRAP_CPP(MODULE_CPPS ${MOC_MODULE_FILES})

include_directories(.)
include_directories(../3dparty/muparser/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(dmtestmodule SHARED ${MODULE_CPPS})

target_link_libraries(dmtestmodule dynamindtoolbox dynamindcore ${QT_LIBRARIES})

INSTALL(TARGETS dmtestmodule
        DESTINATION "lib"
        PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)


