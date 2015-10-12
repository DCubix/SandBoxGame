TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp

LIBS += -lSDL2 -lSDL2main

HEADERS += \
    data_structures.h

