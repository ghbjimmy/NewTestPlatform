#ifndef TPLUGIN_GLOBAL_H
#define TPLUGIN_GLOBAL_H

#include <QtCore/qglobal.h>
#include <QWidget>

#if defined(TPLUGIN_LIBRARY)
#  define TPLUGINSHARED_EXPORT Q_DECL_EXPORT
#else
#  define TPLUGINSHARED_EXPORT Q_DECL_IMPORT
#endif


const int PLUGIN_INIT = 0;       // 初始化插件
const int PLUGIN_FINI = 1;      // 插件释放
const int PLUGIN_GUI  = 10;      // 插件GUI相关消息

class TPLUGINSHARED_EXPORT IMessage
{
public:
    virtual int  MessageID() const = 0;
    virtual void * Context() const = 0;
    virtual int    Length() const = 0;
};

class TPLUGINSHARED_EXPORT IPlugin
{
public:
    virtual int Init() = 0;
    virtual void Fini()  = 0;
    virtual int OnMessage(const IMessage& msg) = 0;
    virtual QWidget * CreateWidget() = 0;
};

typedef IPlugin * (*fnCreatePlugin)();

extern "C" TPLUGINSHARED_EXPORT IPlugin * CreatePlugin();

#endif // TPLUGIN_GLOBAL_H
