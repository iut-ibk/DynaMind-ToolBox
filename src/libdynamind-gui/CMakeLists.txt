
find_package(QGLViewer)
find_package(OpenGL)


find_package(Qt4 COMPONENTS QtCore QtGui QtXml QtOpenGL REQUIRED)


find_package(OpenGL REQUIRED)
set(QT_USE_QTWEBKIT 1)
set(QT_USE_QTXML 1)
set(QT_USE_QTOPENGL 1)
set(QT_USE_QTSQL 1)
set(QT_USE_QTSVG 1)
set(QT_USE_QTNETWORK 1)
include(${QT_USE_FILE})


include_directories(.)
include_directories(../core/)
include_directories(${dynamind_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR}../libdynamind-gui )

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_BINARY_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/libdynamind-gui)



set(MOC_FILES  projectviewer.h guilogsink.h guisimulation.h  mainwindow.h plot.h preferences.h guimodelnode.h modelnode.h guilink.h StdRedirector.h application.h modelnodebutton.h simulationmanagment.h groupnode.h rootgroupnode.h guiresultobserver.h guiimageresultview.h guidataobserver.h)
file (GLOB CPP_GUI_FILES *.cpp)
file (GLOB UI_FILES *.ui)


#set (MOC_FILES datamanagment.h)
QT4_WRAP_UI(CPP_GUI_FILES ${UI_FILES})
QT4_WRAP_CPP(CPP_GUI_FILES ${MOC_FILES})
QT4_ADD_RESOURCES(CPP_GUI_FILES icons.qrc splash.qrc)
file(GLOB QWT_CPPS ../3dparty/qwt-5.2.0/src/*.cpp)
file(GLOB QWT_H ../3dparty/qwt-5.2.0/src/*.h)
QT4_WRAP_CPP(QWT_CPPS ${QWT_H})
#file(GLOB ALGLIB_CPP ../3dparty/alglib/src/*.cpp)

# Find OpenGL
include_directories( ${QGLVIEWER_INCLUDE_DIR})
include_directories( ${QT_QTGUI_INCLUDE_DIR})
include_directories(../3dparty/qwt-5.2.0/src)
include_directories(../3dparty/alglib/src)

add_library(dynamindgui SHARED ${CPP_GUI_FILES}  ${QWT_CPPS} ${QTGL_CPPS} ${ALGLIB_CPP})
target_link_libraries(dynamindgui dynamindcore  ${OPENGL_LIBRARIES} ${QT_LIBRARIES} ${QGLVIEWER_LIBRARIES})


INSTALL(TARGETS dynamindgui
        DESTINATION "lib"
        PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
