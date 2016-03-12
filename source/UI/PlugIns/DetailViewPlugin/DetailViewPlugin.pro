#-------------------------------------------------
#
# Project created by QtCreator 2016-03-09T12:05:47
#
#-------------------------------------------------

QT       += widgets

TARGET = DetailViewPlugin
TEMPLATE = lib

DEFINES += TPLUGIN_LIBRARY

INCLUDEPATH += ../
SOURCES += detailviewplugin.cpp \
    detailviewform.cpp \
    cvsdatatreeview.cpp \
    cvsdatatreemodel.cpp \
    ../Common/datatreemodel.cpp \
    ../Common/datatreenode.cpp \
    cvsdatatreenode.cpp \
    cvsdataadapter.cpp \
    progressform.cpp \
    failform.cpp

HEADERS += detailviewplugin.h\
    detailviewform.h \
    cvsdatatreeview.h \
    cvsdatatreemodel.h \
    ../detailviewsinterface.h \
    ../Common/datatreemodel.h \
    ../Common/datatreenode.h \
    cvsdatatreenode.h \
    cvsdataadapter.h \
    progressform.h \
    failform.h \
    ../../Util/plugin_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
