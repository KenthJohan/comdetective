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
#pkg-config --libs libserialport --static
LIBS += -lserialport -lsetupapi
LIBS += -lws2_32
