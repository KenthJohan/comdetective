TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += static

DEFINES = ""
DEFINES += __USE_MINGW_ANSI_STDIO=1

QMAKE_CFLAGS = ""
QMAKE_CFLAGS += -Wno-unused-function
QMAKE_CFLAGS += -Wno-unused-parameter
QMAKE_CFLAGS += -Wno-missing-braces
QMAKE_CFLAGS += -Wno-missing-field-initializers
QMAKE_CFLAGS += -std=gnu99 -w

INCLUDEPATH += ../third_party/

HEADERS += source/*.h
SOURCES += source/main.c
SOURCES += source/flecs.c
SOURCES += source/eg_serialport.c
SOURCES += source/app_systems.c
SOURCES += source/app_components.c

LIBS += -Lc:/msys64/mingw64/lib
#pkg-config --libs libserialport --static
LIBS += -lserialport -lsetupapi
LIBS += -lws2_32
LIBS += -lopengl32
LIBS += -lkernel32
LIBS += -luser32
LIBS += -lshell32
LIBS += -lgdi32
LIBS += -lAdvapi32
LIBS += -lWinmm
LIBS += -lm
