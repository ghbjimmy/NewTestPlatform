#-------------------------------------------------
#
# Project created by QtCreator 2016-03-22T09:38:12
#
#-------------------------------------------------

QT       += widgets xml

TARGET = FctViewPlugin
TEMPLATE = lib

CONFIG += c++11

DEFINES += TPLUGIN_LIBRARY

INCLUDEPATH += ../ \
                ../common \
                ../../Util \
                ../../Interface \
                ../../Log \
                ../../ZmqCtrl \
                ../../ZmqCtrl/ZeroMQ/include \

SOURCES += fctviewplugin.cpp \
    fctviewform.cpp

HEADERS += fctviewplugin.h \
    fctviewform.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
