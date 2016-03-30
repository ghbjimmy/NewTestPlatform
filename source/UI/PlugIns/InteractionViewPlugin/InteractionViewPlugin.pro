#-------------------------------------------------
#
# Project created by QtCreator 2016-03-11T11:16:37
#
#-------------------------------------------------

QT       += widgets

TARGET = InteractionViewPlugin
TEMPLATE = lib

CONFIG += c++11

DEFINES += TPLUGIN_LIBRARY

INCLUDEPATH += ../ \
                ../common \
                ../../Util \
                ../../Interface \
                ../../Log

SOURCES += interactionviewplugin.cpp \
    interactionviewform.cpp \
    ../Common/uutbutton.cpp \
    ../../Util/util.cpp \
    ../../Util/message.cpp \
    ../../Log/qlog.cpp \
    ../../Util/widgetprivilctrl.cpp

HEADERS += interactionviewplugin.h\
    interactionviewform.h \
    ../Common/uutbutton.h \
    ../../Util/util.h \
    ../../Util/plugin_global.h \
    ../../Util/const.h \
    ../../Util/message.h \
    ../../Log/qlog.h \
    ../../Util/widgetprivilctrl.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS +=
