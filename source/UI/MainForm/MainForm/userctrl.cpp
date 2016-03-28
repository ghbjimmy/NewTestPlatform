#include "userctrl.h"
#include "qlog.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

UserCtrl::UserCtrl()
{

}

UserCtrl::~UserCtrl()
{

}

bool UserCtrl::parseConfig(const QString& path)
{
    QFile* file = new QFile(path);
    if (!file->exists())
    {
        LogMsg(Error, "config file is not exist.[%s]", path.toStdString().c_str());

        return false;
    }

    if (!file->open(QIODevice::ReadOnly))
    {
        LogMsg(Error, "open file failed.[%s]", path.toStdString().c_str());
        return false;
    }

    QByteArray byteArray = file->readAll();
    file->close();

    QJsonParseError json_error;
    QJsonDocument document = QJsonDocument::fromJson(byteArray, &json_error);
    if(json_error.error != QJsonParseError::NoError)
    {
        LogMsg(Error, "Parse json failed.");
        return false;
    }

    QJsonObject obj = document.object();
    if (!obj.value("Users:").isArray())
    {
        LogMsg(Error, "Parse json failed.");
        return false;
    }

    QJsonArray usersArray  = obj.value("Users").toArray();
    int size = usersArray.size();
    for (int i = 0; i < size; ++i)
    {
        QJsonObject userObj = usersArray[i].toObject();
        TUserPrivilege userPri;
        userPri.name = userObj.value("name").toString();
        userPri.password = userObj.value("password").toString();
        userPri.level = userObj.value("level").toInt();

        _userPris.push_back(userPri);
    }

    QJsonArray rightsArray  = obj.value("rights").toArray();
    size = rightsArray.size();
    for (int i = 0; i < size; ++i)
    {
        QJsonObject rightObj = rightsArray[i].toObject();
        int level = rightObj.value("level").toInt();
        QJsonArray wgtArray = rightObj.value("level").toArray();
        int wgtSize = wgtArray.size();
        for (int j = 0; j < wgtSize; ++j)
        {
            QJsonObject wgtObj = wgtArray[j].toObject();

            TWidgetPrivilege widgetPri;
            widgetPri.name = wgtObj.value("name").toString();
            widgetPri.level = level;
            widgetPri.state = wgtObj.value("state").toInt();

            _wgtPrivileges[level].push_back(widgetPri);
        }
    }

    return true;
}

bool UserCtrl::checkUser(const QString& name, const QString& pwd)
{
    return true;
}
