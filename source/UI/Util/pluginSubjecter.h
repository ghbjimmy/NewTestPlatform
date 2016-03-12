#ifndef PLUGINSUBJECTER_H
#define PLUGINSUBJECTER_H

#include "plugin_global.h"
#include <QVector>

class PluginSubjecter
{
public:
    PluginSubjecter();
    ~PluginSubjecter();

    IPlugin* getPlugin(const QString& name);
    void attach(IPlugin* plugin);
    void detach(IPlugin* plugin);
    void notity(const IMessage* msg);

private:
    QVector<IPlugin*> _plugins;

};

#endif
