# webbrowser_static.pri
# 10/9/2011

include(../../config.pri)
include($$ROOTDIR/lib/searchengine/searchengine.pri)

DEFINES += WITH_LIB_WEBBROWSER

INCLUDEPATH += \
    $$PWD
DEPENDPATH += \
    $$PWD \
    $$PWD/core \
    $$PWD/global \
    $$PWD/gui \
    $$PWD/network \
    $$PWD/webkit

#DEFINES +=  WEBBROWSER_STATIC

FORMS   += \
    $$PWD/webbrowser.ui

HEADERS += \
    $$PWD/global/wbrc.h \
    $$PWD/global/wbss.h \
    $$PWD/gui/wbaddressedit.h \
    $$PWD/gui/wbcomboedit.h \
    $$PWD/gui/wblineedit.h \
    $$PWD/gui/wbsearchedit.h \
    $$PWD/gui/wbtabbar.h \
    $$PWD/gui/wbtabwidget.h \
    $$PWD/gui/wbtoolbutton.h \
    $$PWD/network/wbnetworkaccessmanager.h \
    $$PWD/network/wbnetworkcookiejar.h \
    $$PWD/webkit/wbwebview.h \
    $$PWD/webbrowser.h \
    $$PWD/webbrowser_p.h

SOURCES += \
    $$PWD/gui/wbaddressedit.cc \
    $$PWD/gui/wbcomboedit.cc \
    $$PWD/gui/wblineedit.cc \
    $$PWD/gui/wbsearchedit.cc \
    $$PWD/network/wbnetworkaccessmanager.cc \
    $$PWD/network/wbnetworkcookiejar.cc \
    $$PWD/webkit/wbwebview.cc \
    $$PWD/webbrowser.cc

RESOURCES += \
    $$PWD/webbrowser.qrc

DOC_FILES += \
    $$PWD/doc/start.html

OTHER_FILES += \
    $$DOC_FILES

QT += core gui network webkit
#CONFIG(static): QTPLUGIN += qgif qico qjpeg gsvg qtiff

#mac:  LIBS  += -framework WebBrowser

# EOF
