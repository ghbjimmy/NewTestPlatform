#include "pluginSubjecter.h"
#include "message.h"

PluginSubjecter::PluginMsgFilter::PluginMsgFilter()
{

}

PluginSubjecter::PluginMsgFilter::~PluginMsgFilter()
{

}

bool PluginSubjecter::PluginMsgFilter::isNeedProc(int plugState, int msgStatet)
{
    return plugState == msgStatet;
}

PluginSubjecter::PluginSubjecter()
{
    _msgFilter = new PluginSubjecter::PluginMsgFilter();
}

PluginSubjecter::~PluginSubjecter()
{
    foreach(IPlugin* plugin , _plugins)
    {
        plugin->fini();
        delete plugin;
        plugin = NULL;
    }

    _plugins.clear();

    if (NULL != _msgFilter)
    {
        delete _msgFilter;
        _msgFilter = NULL;
    }
}

IPlugin* PluginSubjecter::getPlugin(const QString& name)
{
    IPlugin* plugin = NULL;
    foreach (IPlugin* plug, _plugins)
    {
        if (plug->getName() == name)
        {
            plugin = plug;
            break;
        }
    }

    return plugin;
}

void PluginSubjecter::attach(IPlugin* plugin)
{
    if (!_plugins.contains(plugin))
        _plugins.append(plugin);
}

void PluginSubjecter::detach(IPlugin* plugin)
{
    if (_plugins.contains(plugin))
        _plugins.removeOne(plugin);
}

void PluginSubjecter::notity(const IMessage* msg)
{
    foreach(IPlugin* plugin , _plugins)
    {
        //消息状态过滤
        if (!_msgFilter->isNeedProc(plugin->getStrategyID(), msg->strategyID()))
            continue;

        if (plugin->isHandleMessage(msg))
        {
            int result = plugin->onMessage(msg);
        }
    }
}
