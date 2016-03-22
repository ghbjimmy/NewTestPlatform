#-------------------------------------------------
#
# Project created by QtCreator 2016-03-22T09:38:12
#
#-------------------------------------------------

QT       += widgets xml

TARGET = FctViewPlugin
TEMPLATE = lib

DEFINES += FCTVIEWPLUGIN_LIBRARY

INCLUDEPATH += ../ \
                ../common \
                ../../Util \
                ../../Interface \
                ../../Log \
                ../../ZmqCtrl \
                ../../ZmqCtrl/ZeroMQ/include \

SOURCES += fctviewplugin.cpp

HEADERS += fctviewplugin.h\
        fctviewplugin_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
