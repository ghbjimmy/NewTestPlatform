#ifndef PLUGINSUBJECTER_H
#define PLUGINSUBJECTER_H

#include "plugin_global.h"
#include <QVector>

//插件管理类
class PluginSubjecter
{
    //插件消息过滤 消息的类型必须要和插件的状态一致才需要处理
    class PluginMsgFilter
    {
    public:
        PluginMsgFilter();
        ~PluginMsgFilter();

        bool isNeedProc(int plugState, int msgStatet);

    private:

    };

public:
    PluginSubjecter();
    ~PluginSubjecter();

    IPlugin* getPlugin(const QString& name);
    void attach(IPlugin* plugin);
    void detach(IPlugin* plugin);
    void notity(const IMessage* msg);

private:
    QVector<IPlugin*> _plugins;
    PluginMsgFilter* _msgFilter;

};

#endif
