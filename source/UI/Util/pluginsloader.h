#ifndef PLUGINSLOADER_H
#define PLUGINSLOADER_H

#include <QString>
#include <QVector>

//负责插件xml的解析并且加载插件

struct TPluginInfo
{
    QString name;
    bool isLoad;
    QString path;
    bool isExportMenu;
    QString menuName;
};

class PluginsLoader
{

public:
    PluginsLoader();
    ~PluginsLoader();

    bool parseXml();
    inline const QVector<TPluginInfo>& getPluginInfos() const { return _plugInInfos;}

private:
    QVector<TPluginInfo> _plugInInfos;

};

#endif // PLUGINSLOADER_H
