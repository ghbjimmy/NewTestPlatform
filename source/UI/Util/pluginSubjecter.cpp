#include "pluginSubjecter.h"

PluginSubjecter::PluginSubjecter()
{

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
        if (plugin->isHandleMessage(msg))
        {
            int result = plugin->onMessage(msg);
        }
    }
}
