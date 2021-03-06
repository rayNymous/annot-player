# x.pro
# 11/30/2011
## Compiler -- TODO: move to config.pri

include(../../config.pri)

## Library

LIBS += -lssh2
QT -= core gui

## Application

TEMPLATE = app
mac: CONFIG -= app_bundle

TARGET =
DEPENDPATH += .
INCLUDEPATH += .

HEADERS += x.h
SOURCES += main.cc

OTHER_FILES += Makefile

# EOF

#win32 {
#    CYGWIN_CC   = i686-pc-cygwin-gcc-4
#    CYGWIN_CXX  = i686-pc-cygwin-g++-4
#
#    #QMAKESPEC = cygwin-g++
#
#    CONFIG   -= qt embed_manifest_dll embed_manifest_exe
#
#    DEFINES     -= UNICODE WIN32 QT_LARGEFILE_SUPPORT
#    QMAKE_COMPILER_DEFINES  -= _MSC_VER=1600 WIN32
#
#    QMAKE_CC =  $$CYGWIN_CC
#    QMAKE_CXX = $$CYGWIN_CXX
#
#    QMAKE_CXXFLAGS =
#    QMAKE_CXXFLAGS_DEBUG=
#    QMAKE_CXXFLAGS_MT=
#    QMAKE_CXXFLAGS_MT_DBG=
#    QMAKE_CXXFLAGS_MT_DLL=
#    QMAKE_CXXFLAGS_MT_DLLDBG=
#    QMAKE_CXXFLAGS_RELEASE=
#    QMAKE_CXXFLAGS_SHLIB=
#    QMAKE_CXXFLAGS_THREAD=
#    QMAKE_CXXFLAGS_WARN_OFF=
#    QMAKE_CXXFLAGS_WARN_ON=
#
#    QMAKE_CFLAGS = -pipe
#    QMAKE_CFLAGS_DEPS = -M
#    QMAKE_CFLAGS_WARN_ON    = -Wall -W
#    QMAKE_CFLAGS_WARN_OFF   = -w
#    QMAKE_CFLAGS_RELEASE    =  -O2
#    QMAKE_CFLAGS_RELEASE_WITH_DEBUGINFO +=
#    QMAKE_CFLAGS_DEBUG      = -g
#    QMAKE_CFLAGS_SHLIB =
#    QMAKE_CFLAGS_LTCG       =
#    QMAKE_CFLAGS_YACC  = -Wno-unused -Wno-parentheses
#    QMAKE_CFLAGS_MP         =
#    QMAKE_CFLAGS_THREAD     = -D_REENTRANT
#
#    QMAKE_CXX               =
#    QMAKE_CXXFLAGS          = $$QMAKE_CFLAGS
#    QMAKE_CXXFLAGS_DEPS	= $$QMAKE_CFLAGS_DEPS
#    QMAKE_CXXFLAGS_RELEASE_WITH_DEBUGINFO =
#    QMAKE_CXXFLAGS_WARN_ON      = $$QMAKE_CFLAGS_WARN_ON
#    QMAKE_CXXFLAGS_WARN_OFF     = $$QMAKE_CFLAGS_WARN_OFF
#    QMAKE_CXXFLAGS_STL_ON   =
#    QMAKE_CXXFLAGS_RTTI_ON  =
#    QMAKE_CXXFLAGS_EXCEPTIONS_ON =
#    QMAKE_CXXFLAGS_RELEASE      = $$QMAKE_CFLAGS_RELEASE
#    QMAKE_CXXFLAGS_DEBUG        = $$QMAKE_CFLAGS_DEBUG
#    QMAKE_CXXFLAGS_SHLIB        = $$QMAKE_CFLAGS_SHLIB
#    QMAKE_CXXFLAGS_YACC	= $$QMAKE_CFLAGS_YACC
#    QMAKE_CXXFLAGS_THREAD       = $$QMAKE_CFLAGS_THREAD
#    QMAKE_CXXFLAGS_LTCG     =
#    QMAKE_CXXFLAGS_MP       =
#
#    QMAKE_LFLAGS_RELEASE_WITH_DEBUGINFO =
#    QMAKE_LFLAGS_CONSOLE    =
#    QMAKE_LFLAGS_WINDOWS    =
#    QMAKE_LFLAGS_DLL        =
#    QMAKE_LFLAGS_LTCG       =
#
#    QMAKE_LINK          = $$QMAKE_CXX
#    QMAKE_LINK_SHLIB    = $$QMAKE_CXX
#    QMAKE_LINK_C        = $$QMAKE_CC
#    QMAKE_LINK_C_SHLIB  = $$QMAKE_CC
#    QMAKE_LFLAGS        =
#    QMAKE_LFLAGS_RELEASE=
#    QMAKE_LFLAGS_DEBUG  =
#    QMAKE_LFLAGS_SHLIB  = -shared
#    QMAKE_LFLAGS_PLUGIN = $$QMAKE_LFLAGS_SHLIB
#    QMAKE_LFLAGS_SONAME = -Wl,-soname,
#    QMAKE_LFLAGS_THREAD =
#    QMAKE_LFLAGS_RPATH  = -Wl,-rpath,
#    QMAKE_CYGWIN_SHLIB  = 1
#    QMAKE_CYGWIN_EXE    = 1
#
#    QMAKE_RUN_CC            = $$QMAKE_CC  -c $(CFLAGS) $(INCPATH) -o$@ $src
#    QMAKE_RUN_CC_IMP        = $$QMAKE_CC  -c $(CFLAGS) $(INCPATH) -o$@ $<
#    QMAKE_RUN_CC_IMP_BATCH  = $$QMAKE_CC  -c $(CFLAGS) $(INCPATH) -o$@ @<<
#    QMAKE_RUN_CXX           = $$QMAKE_CXX -c $(CXXFLAGS) $(INCPATH) -o$@ $src
#    QMAKE_RUN_CXX_IMP       = $$QMAKE_CXX -c $(CXXFLAGS) $(INCPATH) -o$@ $<
#    QMAKE_RUN_CXX_IMP_BATCH = $$QMAKE_CXX -c $(CXXFLAGS) $(INCPATH) -o$@ @<<
#
#    QMAKE_RUN_CC            = i686-pc-cygwin-gcc-4 -c $(CFLAGS) $(INCPATH) -o$@ $<
#    QMAKE_RUN_CC_IMP        = i686-pc-cygwin-gcc-4 -c $(CFLAGS) $(INCPATH) -o$@ $<
#    QMAKE_RUN_CC_IMP_BATCH  = i686-pc-cygwin-gcc-4 -c $(CFLAGS) $(INCPATH) -o$@ @<<
#    QMAKE_RUN_CXX           = i686-pc-cygwin-g++-4 -c $(CXXFLAGS) $(INCPATH) -o$@ $<
#    QMAKE_RUN_CXX_IMP       = i686-pc-cygwin-g++-4 -c $(CXXFLAGS) $(INCPATH) -o$@ $<
#    QMAKE_RUN_CXX_IMP_BATCH = i686-pc-cygwin-g++-4 -c $(CXXFLAGS) $(INCPATH) -o$@ @<<
#}
