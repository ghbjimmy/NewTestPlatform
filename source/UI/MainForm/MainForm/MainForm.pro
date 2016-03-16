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
                ../../Log \
                ../../Interface \
                ../../ZmqCtrl

LIBS += D:/Work/tm_platform_new/source/UI/ZmqCtrl/ZeroMQ/lib/libzmq-v120-mt-4_0_4.lib

SOURCES += main.cpp\
        mainwindow.cpp \
    ../../Util/libaryparser.cpp \
    ../../Util/pluginSubjecter.cpp \
    ../../Util/util.cpp \
    ../../Util/message.cpp \
    ../../Log/qlog.cpp \
    ../../ZmqCtrl/zmqsocket.cpp \
    ../../ZmqCtrl/buffer.cpp \
    ../../ZmqCtrl/zmqcfgparser.cpp \
    ../../ZmqCtrl/command.cpp \
    ../../ZmqCtrl/sequencerrpc.cpp \

HEADERS  += mainwindow.h \
    ../../Util/libaryparser.h \
    ../../Util/plugin_global.h \
    ../../Util/pluginSubjecter.h \
    ../../Util/util.h \
    ../../Util/const.h \
    ../../Util/message.h \
    ../../Log/qlog.h \
    ../../ZmqCtrl/zmqsocket.h \
    ../../ZmqCtrl/buffer.h \
    ../../ZmqCtrl/zmqcfgparser.h \
    ../../ZmqCtrl/command.h \
    ../../ZmqCtrl/sequencerrpc.h \
    ../../Interface/structdefine.h

FORMS    +=

RESOURCES += \
    mainform.qrc
