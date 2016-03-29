#include "pluginsloader.h"
#include "qlog.h"

#include <QDomDocument>
#include <QFile>
#include <QApplication>

PluginsLoader::PluginsLoader()
{

}

PluginsLoader::~PluginsLoader()
{

}

bool PluginsLoader::parseXml()
{
    QString file_name = QApplication::applicationDirPath() + "/config/" + "modulecfg.xml";
    QFile file(file_name);
    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        LogMsg(Error, "open file failed. %s", file_name.toStdString().c_str());
        return false;
    }

    QDomDocument dom;
    QString error;
    int row = 0, column = 0;
    if(!dom.setContent(&file, false, &error, &row, &column))
    {
        LogMsg(Error, "open xml file failed1. %s", file_name.toStdString().c_str());
        return false;
    }

    file.close();
    if(dom.isNull())
    {
        LogMsg(Error, "open xml file failed. %s", file_name.toStdString().c_str());
        return false;
    }

    QDomElement root = dom.documentElement();
    QDomNodeList moduleNodes =  root.childNodes();
    for (int i = 0; i < moduleNodes.size(); ++i)
    {
        TPluginInfo info;
        QDomElement node = moduleNodes.at(i).toElement();
        info.name = node.attribute("name");
        info.isLoad = node.attribute("isLoad")
                .compare("true", Qt::CaseInsensitive) == 0 ? true : false;

        QDomNodeList childNodes = node.childNodes();
        for (int j = 0; j < childNodes.size(); ++j)
        {
            QDomElement child = childNodes.at(j).toElement();
            if (child.tagName() == "Path")
            {
                info.path = child.text() + "/" + info.name;
            }
            else if (child.tagName() == "Actions")
            {
                info.isExportMenu = child.attribute("isExport")
                        .compare("true", Qt::CaseInsensitive) == 0 ? true : false;
                info.menuName = child.text();
            }
        }

        _plugInInfos.push_back(info);
    }

    return true;
}

