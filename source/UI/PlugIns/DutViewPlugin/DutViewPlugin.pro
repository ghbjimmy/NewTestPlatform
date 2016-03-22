#-------------------------------------------------
#
# Project created by QtCreator 2016-03-22T09:31:46
#
#-------------------------------------------------

QT       += widgets xml

TARGET = DutViewPlugin
TEMPLATE = lib

DEFINES += TPLUGIN_LIBRARY

LIBS += D:/Work/tm_platform_new/source/UI/ZmqCtrl/ZeroMQ/lib/libzmq-v120-mt-4_0_4.lib

INCLUDEPATH += ../ \
                ../common \
                ../../Util \
                ../../Interface \
                ../../Log \
                ../../ZmqCtrl \
                ../../ZmqCtrl/ZeroMQ/include \

SOURCES += dutviewplugin.cpp \
    dutviewform.cpp \
    ../../Log/qlog.cpp \
    dutviewconfigdlg.cpp \
    dutzmqrpc.cpp \
    ../../ZmqCtrl/zmqsocket.cpp \
    ../../ZmqCtrl/buffer.cpp

HEADERS += dutviewplugin.h\
    ../../Util/plugin_global.h \
    dutviewform.h \
    ../../Log/qlog.h \
    dutviewconfigdlg.h \
    dutzmqrpc.h \
    ../../ZmqCtrl/zmqsocket.h \
    ../../ZmqCtrl/buffer.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS +=
