TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_CXXFLAGS += -std=c++0x
QMAKE_LIBS += -pthread
QMAKE_LIBS += -lrt

SOURCES += main.cpp
