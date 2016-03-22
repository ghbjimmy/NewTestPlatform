#ifndef TPLUGIN_GLOBAL_H
#define TPLUGIN_GLOBAL_H

#include <QtCore/qglobal.h>
#include <QWidget>

#if defined(TPLUGIN_LIBRARY)
#  define TPLUGINSHARED_EXPORT Q_DECL_EXPORT
#else
#  define TPLUGINSHARED_EXPORT Q_DECL_IMPORT
#endif


class IMessage;
typedef void (*fnSendMsg)(const IMessage* msg);

class TPLUGINSHARED_EXPORT IPlugin
{
public:
    IPlugin() {_name = "unknown"; _sendCallback = NULL;}
    virtual ~IPlugin(){}

    virtual void registerSendMsgCallBack(fnSendMsg callback) {_sendCallback = callback;}
    virtual void sendMessage(const IMessage* msg) {if (_sendCallback != NULL) _sendCallback(msg);}
    virtual const QString& getName() const { return _name; }
    virtual int init() = 0;
    virtual void fini()  = 0;
    virtual int onMessage(const IMessage* msg) = 0;
    virtual bool isHandleMessage(const IMessage* msg) = 0;
    virtual QWidget * createWidget() = 0;

protected:
    QString _name;
    fnSendMsg _sendCallback;
};

typedef IPlugin * (*fnCreatePlugin)();

extern "C" TPLUGINSHARED_EXPORT IPlugin * createPlugin();

#endif // TPLUGIN_GLOBAL_H
