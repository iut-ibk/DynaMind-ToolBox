INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_BINARY_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

SET(MOC_FILES   application.h
                groupnode.h
                guidataobserver.h
                guiimageresultview.h
                guilink.h
                guilogsink.h
                guimodelnode.h
                guiresultobserver.h
                guisimulation.h
                mainwindow.h
                modelnode.h
                modelnodebutton.h
                preferences.h
                projectviewer.h
                rootgroupnode.h
                simulationmanagment.h
                StdRedirector.h )

FILE(GLOB CPP_GUI_FILES *.cpp)
FILE(GLOB UI_FILES *.ui)

QT4_WRAP_UI(CPP_GUI_FILES ${UI_FILES})
QT4_WRAP_CPP(CPP_GUI_FILES ${MOC_FILES})
QT4_ADD_RESOURCES(CPP_GUI_FILES icons.qrc splash.qrc)

FILE(GLOB QWT_CPPS ../3dparty/qwt-5.2.0/src/*.cpp)
FILE(GLOB QWT_H ../3dparty/qwt-5.2.0/src/*.h)

INCLUDE_DIRECTORIES( ${QGLVIEWER_INCLUDE_DIR})
INCLUDE_DIRECTORIES( ${QT_QTGUI_INCLUDE_DIR})
INCLUDE_DIRECTORIES(../3dparty/qwt-5.2.0/src)
INCLUDE_DIRECTORIES(../3dparty/alglib/src)

ADD_LIBRARY(dynamindgui STATIC ${CPP_GUI_FILES}  ${QWT_CPPS} ${QTGL_CPPS} ${ALGLIB_CPP})


IF (${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
       TARGET_LINK_LIBRARIES(dynamindgui ${DYNAMINDCORE_LIBRARY} ${DANYMINDTOOLBOX_LIBRARY} ${QT_QTGUI_LIBRARY} ${Boost_PROGRAM_OPTIONS_LIBRARY} ${QT_QTMAIN_LIBRARY} ${OPENGL_LIBRARIES} ${QT_LIBRARIES} ${QGLVIEWER_LIBRARIES})
ELSE()
       TARGET_LINK_LIBRARIES(dynamindgui ${DYNAMINDCORE_LIBRARY} ${DANYMINDTOOLBOX_LIBRARY} ${QT_QTMAIN_LIBRARY} ${OPENGL_LIBRARIES} ${QT_LIBRARIES} ${QGLVIEWER_LIBRARIES})
ENDIF()


INSTALL(TARGETS dynamindgui
        DESTINATION "lib"
        PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
