TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

DEFINES = ""
DEFINES += __USE_MINGW_ANSI_STDIO=1

QMAKE_CFLAGS = ""
QMAKE_CFLAGS += -Wno-unused-function
QMAKE_CFLAGS += -Wno-unused-parameter
QMAKE_CFLAGS += -Wno-missing-braces
QMAKE_CFLAGS += -Wno-missing-field-initializers
QMAKE_CFLAGS += -std=gnu99 -w

INCLUDEPATH += ../third_party/

SOURCES += source/main.c


LIBS += -lws2_32
LIBS += -lopengl32
LIBS += -lkernel32
LIBS += -luser32
LIBS += -lshell32
LIBS += -lgdi32
LIBS += -lAdvapi32
LIBS += -lWinmm
LIBS += -lm
