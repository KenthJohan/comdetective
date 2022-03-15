TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += static

SOURCES += main.c




LIBS += -Lc:/msys64/mingw64/lib
#pkg-config --libs libserialport --static
LIBS += -lserialport -lsetupapi -lusb-1.0
LIBS += -lnng
LIBS += -lws2_32 -lmswsock -ladvapi32 -lkernel32 -luser32 -lgdi32 -lwinspool -lshell32 -lole32 -loleaut32 -luuid -lcomdlg32 -ladvapi32
