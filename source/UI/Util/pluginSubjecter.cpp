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
