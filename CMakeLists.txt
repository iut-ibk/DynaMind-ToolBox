cmake_minimum_required(VERSION 2.6)
project(vibe2core)

set(Boost_USE_STATIC_LIBS   OFF)
find_package(Boost 1.38.0 COMPONENTS python program_options date_time REQUIRED)
find_package(PythonLibs REQUIRED)
find_package(Qt4 COMPONENTS QtCore)

set(QT_USE_QTXML 1)
include(${QT_USE_FILE})


include_directories(src/core ${Boost_INCLUDE_DIRS} ${QT_QTCORE_INCLUDE_DIR} ${PYTHON_INCLUDE_PATH})
set(LIBRARY_OUTPUT_PATH ${vibe2core_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${vibe2core_BINARY_DIR})


add_subdirectory(src/core)
add_subdirectory(src/vibe)


