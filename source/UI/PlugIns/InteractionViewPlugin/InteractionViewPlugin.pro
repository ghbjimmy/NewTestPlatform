#-------------------------------------------------
#
# Project created by QtCreator 2016-03-11T11:16:37
#
#-------------------------------------------------

QT       += widgets

TARGET = InteractionViewPlugin
TEMPLATE = lib

DEFINES += TPLUGIN_LIBRARY

SOURCES += interactionviewplugin.cpp \
    interactionviewform.cpp \
    ../Common/uutbutton.cpp \
    ../../Util/util.cpp

HEADERS += interactionviewplugin.h\
    interactionviewform.h \
    ../Common/uutbutton.h \
    ../../Util/util.h \
    ../../Util/plugin_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS +=
