TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += comdetective.c
SOURCES += argparse.c
HEADERS += argparse.h

INCLUDEPATH += "C:\msys64\mingw64\include"

LIBS += -L"C:\msys64\mingw64\lib"
LIBS += -lserialport
