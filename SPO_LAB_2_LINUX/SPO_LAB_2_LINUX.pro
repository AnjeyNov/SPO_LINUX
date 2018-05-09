TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp

NCLUDEPATH += /path/to/ncurses/headers/
LIBS += -L/path/to/ncurses/library/ -lncurses
