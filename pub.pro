TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += pub.c
SOURCES += argparse.c
HEADERS += argparse.h
HEADERS += ccc/ccc_tcol.h
HEADERS += ccc/ccc_debug.h
HEADERS += comdetective.h

INCLUDEPATH += C:/msys64/mingw64/include
INCLUDEPATH += $$PWD/nng/include
INCLUDEPATH += $$PWD/ccc

LIBS += -Lc:/msys64/mingw64/lib
LIBS += -lserialport
LIBS += -L$$PWD/nng/lib
LIBS += -lnng
LIBS += -lws2_32 -lmswsock -ladvapi32 -lkernel32 -luser32 -lgdi32 -lwinspool -lshell32 -lole32 -loleaut32 -luuid -lcomdlg32 -ladvapi32
