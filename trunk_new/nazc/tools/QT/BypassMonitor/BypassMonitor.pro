#-------------------------------------------------
#
# Project created by QtCreator 2011-11-04T13:18:51
#
#-------------------------------------------------

QT       += core gui

TARGET = BypassMonitor
TEMPLATE = app


SOURCES += main.cpp\
        BypassMonitor.cpp \
    Variable.cpp \
    Utils.cpp \
    nzcserver.cpp \
    CommandHandler.cpp

HEADERS  += BypassMonitor.h \
    Utils.h \
    nzcserver.h \
    CommandHandler.h

FORMS    += BypassMonitor.ui

DEFINES += _WIN32

INCLUDEPATH += ../../../include \
    ../../../include/common \
    ../../../include/if4api \
    ../../../include/varapi
LIBS += ../../../lib/libif4api-mingw32.a \
    ../../../lib/libvarapi-mingw32.a \
    ../../../lib/libcommon-mingw32.a \
    ../../../lib/libz-mingw32.a \
    -lpthread \
    -lwsock32
