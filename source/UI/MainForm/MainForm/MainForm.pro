#-------------------------------------------------
#
# Project created by QtCreator 2016-03-09T14:08:25
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MainForm
TEMPLATE = app


INCLUDEPATH += ../../Util \
                ../../ZmqCtrl/ZeroMQ/include \
                ../../Log

LIBS += D:/Work/tm_platform_new/source/UI/ZmqCtrl/ZeroMQ/lib/libzmq-v120-mt-gd-4_0_4.lib

SOURCES += main.cpp\
        mainwindow.cpp \
    ../../Util/libaryparser.cpp \
    ../../Util/pluginSubjecter.cpp \
    ../../Util/util.cpp \
    ../../Util/message.cpp \
    ../../ZmqCtrl/ZeroMQ/zmqcfgparser.cpp \
    ../../Log/qlog.cpp

HEADERS  += mainwindow.h \
    ../../Util/libaryparser.h \
    ../../Util/plugin_global.h \
    ../../Util/pluginSubjecter.h \
    ../../Util/util.h \
    ../../Util/const.h \
    ../../Util/message.h \
    ../../ZmqCtrl/ZeroMQ/zmqcfgparser.h \
    ../../Log/qlog.h

FORMS    +=
