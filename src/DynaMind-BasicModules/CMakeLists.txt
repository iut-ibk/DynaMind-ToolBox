FILE(GLOB MODULE_CPPS *.cpp)
FILE(GLOB MODULE_UI_FILES *.ui)
FILE(GLOB MODULE_CPPS *.cpp ../3dparty/muparser/src/*.cpp)
SET(MOC_MODULE_FILES guidynamicinout.h
                     guigrouptest.h
                     guicellularautomata.h
                     guicellularautomata_neighbourhood.h
                     guiequationeditor.h
                     guiadddatatonewview.h
                     guiexportrasterdata.h
                     guiappendviewfromsystem.h
                     guimarker.h
                     guiappendattributes.h
                     guiclearview.h
                     guicalculatecentroid.h
                     guiattributecalculator.h)

QT4_WRAP_UI(MODULE_CPPS ${MODULE_UI_FILES})
QT4_WRAP_CPP(MODULE_CPPS ${MOC_MODULE_FILES})

INCLUDE_DIRECTORIES(.)
INCLUDE_DIRECTORIES(../3dparty/muparser/include)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

ADD_LIBRARY(dynamind-basicmodules SHARED ${MODULE_CPPS})

TARGET_LINK_LIBRARIES(dynamind-basicmodules  ${DYNAMIND_TOOLBOX_LIBRARY}  ${DYNAMIND_CORE_LIBRARY} ${QT_LIBRARIES})

INSTALL(TARGETS dynamind-basicmodules
        DESTINATION "lib/DynaMind"
        PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)


