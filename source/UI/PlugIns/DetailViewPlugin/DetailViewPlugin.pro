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
    detailviewform.cpp

HEADERS += detailviewplugin.h\
    detailviewform.h \

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS +=
