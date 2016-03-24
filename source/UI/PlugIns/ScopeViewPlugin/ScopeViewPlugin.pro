#-------------------------------------------------
#
# Project created by QtCreator 2016-03-10T15:16:08
#
#-------------------------------------------------

QT       += widgets

TARGET = ScopeViewPlugin
TEMPLATE = lib

CONFIG += c++11

DEFINES += TPLUGIN_LIBRARY

INCLUDEPATH += ../ \
                ../common \
                ../../Util \
                ../../Interface \
                ../../Log

SOURCES += scopeviewplugin.cpp \
    scopeviewform.cpp \
    ../Common/uutbutton.cpp \
    ../../Util/util.cpp \
    ../../Util/message.cpp \
    ../../Log/qlog.cpp

HEADERS += scopeviewplugin.h \
    scopeviewform.h \
    ../Common/uutbutton.h \
    ../../Util/util.h \
    ../../Util/plugin_global.h \
    ../../Util/message.h \
    ../../Log/qlog.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS +=
