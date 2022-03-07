TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += static

SOURCES += comdetective.c
SOURCES += ../csc/argparse.c
HEADERS += ../csc/argparse.h
HEADERS += ../csc/csc_tcol.h
HEADERS += ../csc/csc_debug.h
HEADERS += comdetective.h

INCLUDEPATH += C:/msys64/mingw64/include
INCLUDEPATH += ../nng/include
INCLUDEPATH += ../csc

LIBS += -Lc:/msys64/mingw64/lib
#pkg-config --libs libserialport --static
LIBS += -lserialport -lsetupapi
LIBS += -L$$PWD/nng/lib
LIBS += -lnng
LIBS += -lws2_32 -lmswsock -ladvapi32 -lkernel32 -luser32 -lgdi32 -lwinspool -lshell32 -lole32 -loleaut32 -luuid -lcomdlg32 -ladvapi32
