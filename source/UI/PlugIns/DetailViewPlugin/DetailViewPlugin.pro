#-------------------------------------------------
#
# Project created by QtCreator 2016-03-09T12:05:47
#
#-------------------------------------------------

QT       += widgets

TARGET = DetailViewPlugin
TEMPLATE = lib

CONFIG += c++11
DEFINES += TPLUGIN_LIBRARY

INCLUDEPATH += ../ \
                ../common \
                ../../Util \
                ../../Interface \
                ../../Log

SOURCES += detailviewplugin.cpp \
    detailviewform.cpp \
    cvsdatatreeview.cpp \
    cvsdatatreemodel.cpp \
    ../Common/datatreemodel.cpp \
    ../Common/datatreenode.cpp \
    cvsdatatreenode.cpp \
    cvsdataadapter.cpp \
    progressform.cpp \
    failform.cpp \
    cvsdatatreedelegate.cpp \
    ../../Util/message.cpp \
    ../../Interface/structdefine.cpp \
    ../../Log/qlog.cpp

HEADERS += detailviewplugin.h\
    detailviewform.h \
    cvsdatatreeview.h \
    cvsdatatreemodel.h \
    ../Common/datatreemodel.h \
    ../Common/datatreenode.h \
    cvsdatatreenode.h \
    cvsdataadapter.h \
    progressform.h \
    failform.h \
    ../../Util/plugin_global.h \
    cvsdatatreedelegate.h \
    ../../Util/message.h \
    detailviewsinterface.h \
    ../../Interface/structdefine.h \
    ../../Log/qlog.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
