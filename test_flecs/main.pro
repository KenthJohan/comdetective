TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += static

SOURCES += main.c
SOURCES += flecs.c

INCLUDEPATH += C:/msys64/mingw64/include
HEADERS += *.h


LIBS += -Lc:/msys64/mingw64/lib
LIBS += -lws2_32
