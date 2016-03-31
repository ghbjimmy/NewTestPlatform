#ifndef TPLUGIN_GLOBAL_H
#define TPLUGIN_GLOBAL_H

#include <QtCore/qglobal.h>
#include <QWidget>
#include <QVector>
#include <QAction>


#if defined(TPLUGIN_LIBRARY)
#  define TPLUGINSHARED_EXPORT Q_DECL_EXPORT
#else
#  define TPLUGINSHARED_EXPORT Q_DECL_IMPORT
#endif


class IMessage;
class IPlugin;

typedef void (*fnSendMsg)(const IMessage* msg);

class TPLUGINSHARED_EXPORT IModuleForm : public QWidget
{
public:
    IModuleForm(IPlugin* plugIn, QWidget* parent = 0): QWidget(parent) { _plugin = plugIn; _ownerWgt = NULL;}
	virtual ~IModuleForm() {}

    virtual void setOwner(QWidget* owner) { _ownerWgt = owner; }

    virtual bool init() = 0;
    virtual void clear() = 0;
    virtual QVector<QAction*> getActions() = 0;

protected:
    IPlugin* _plugin;
    QWidget* _ownerWgt; //设置所属窗口
};

//插件的类型，跟消息的type对应，一个插件可以处理多个类型的消息，如 Main | Device
enum TPLUGINSHARED_EXPORT EPluginType
{
    Main = 0x10, //主插件
    Device = 0x20, //设备相关
    Tool = 0x40,   //工具相关
    ALL = 0xFFFF   //全部都要处理
};

class TPLUGINSHARED_EXPORT IPlugin
{
public:
    IPlugin() {_name = "unknown"; _sendCallback = NULL; _pluginType = ALL; _pluginStrategyID = 0;}
    virtual ~IPlugin(){}

    virtual void registerSendMsgCallBack(fnSendMsg callback) {_sendCallback = callback;}
    virtual void sendMessage(const IMessage* msg) {if (_sendCallback != NULL) _sendCallback(msg);}
    virtual const QString& getName() const { return _name; }
    virtual int init() = 0;
    virtual void fini()  = 0;
    virtual int onMessage(const IMessage* msg) = 0;
    virtual bool isHandleMessage(const IMessage* msg) = 0;
    virtual IModuleForm* getModuleForm() = 0;

    virtual int getStrategyID() const { return _pluginStrategyID;}
    virtual void setStrategyID(int state) {_pluginStrategyID = state;}

protected:
    IModuleForm* _widget;
    QString _name;
    fnSendMsg _sendCallback;
    int _pluginType;
    int _pluginStrategyID; //0: inactive; 1: active
};

typedef IPlugin * (*fnCreatePlugin)();

extern "C" TPLUGINSHARED_EXPORT IPlugin * createPlugin();

#endif // TPLUGIN_GLOBAL_H
