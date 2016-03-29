﻿#ifndef TPLUGIN_GLOBAL_H
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
    virtual IModuleForm* getModuleForm() = 0;

protected:
    IModuleForm* _widget;
    QString _name;
    fnSendMsg _sendCallback;
};

typedef IPlugin * (*fnCreatePlugin)();

extern "C" TPLUGINSHARED_EXPORT IPlugin * createPlugin();

#endif // TPLUGIN_GLOBAL_H
