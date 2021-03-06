# tester.pro
# 5/6/2012

VERSION = 0.1.0.0

DEFINES += SRC_TESTER

include(../../config.pri)
include($$ROOTDIR/lib/download/download.pri)

## Libraries
#include($$ROOTDIR/lib/graphicseffect/graphicseffect.pri)

#win32 {
#    include($$ROOTDIR/win/dwm/dwm.pri)
#    include($$ROOTDIR/win/qtwin/qtwin.pri)
#}
#unix: {
#    include($$ROOTDIR/unix/qtunix/qtunix.pri)
#}
#mac {
#    include($$ROOTDIR/mac/qtmac/qtmac.pri)
#}

## Sources

TEMPLATE      = app
win32:  TARGET = annot-tester
unix:   TARGET = annot-tester
mac:   TARGET = "Annot Tester"

win32: CONFIG += windows

INCLUDEPATH     += $$SUBPATH
DEPENDPATH      += $$SUBPATH

HEADERS += \
    window.h
SOURCES += \
    main.cc

#RESOURCES += tester.qrc

#win32 {
#    !wince*: LIBS += -lshell32
#    RC_FILE += tester.rc
#}
#
#mac {
#    ICON = tester.icns
#    QMAKE_INFO_PLIST = Info.plist
#}

CODECFORTR = UTF-8

# EOF
