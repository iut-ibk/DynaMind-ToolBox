QT -= gui
QT += xml
TARGET = vibe2
CONFIG += console
CONFIG -= app_bundle
QMAKE_CXXFLAGS += -fopenmp \
    $$system(python-config --cflags)
LIBS += -lgomp \
    $$system(python-config --ldflags) \
    -lboost_python
SOURCES += main.cpp \
    module.cpp \
    simulation.cpp \
    rasterdata.cpp \
    simulationreader.cpp \
    rootmodule.cpp \
    moduleregistry.cpp \
    moduleparameterreader.cpp \
    moduleloader.cpp \
    simulationwrapper.cpp \
    vectordata.cpp \
    modulecontainer.cpp
HEADERS += module.h \
    simulation.h \
    rasterdata.h \
    simulationreader.h \
    rootmodule.h \
    moduleregistry.h \
    moduleparameterreader.h \
    moduleloader.h \
    vectordata.h \
    modulecontainer.h \
    wrapperhelper.h
