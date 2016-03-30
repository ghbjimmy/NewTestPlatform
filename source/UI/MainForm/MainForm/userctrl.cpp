#include "userctrl.h"
#include "qlog.h"
#include "util.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

UserCtrl::UserCtrl()
{
    _curUserIndex = -1;
}

UserCtrl::~UserCtrl()
{

}

bool UserCtrl::init()
{
    /*QString path = "";
    if (!parseConfig(path))
    {
        LogMsg(Error, "init user file failed.[%s]", path.toStdString().c_str());
        return false;
    }*/

    return true;
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

bool UserCtrl::login(const QString& name, const QString& pwd, QMap<QString, int>& widgetPrivils)
{
    bool ret = false;
    QString md5Pwd = UIUtil::EncryptMd5(pwd);
    for (int i = 0; i < _userPris.size(); ++i)
    {
        const TUserPrivilege& userPri = _userPris[i];
        if (userPri.name == name && userPri.password == md5Pwd)
        {
            _curUserIndex = i;
            int level = userPri.level;
            for (auto iter = _wgtPrivileges.begin(); iter != _wgtPrivileges.end(); ++iter)
            {
                if (iter.key() == level)
                {
                    const QVector<TWidgetPrivilege>& vecs = iter.value();
                    for (int j = 0; j < vecs.size(); ++j)
                    {
                        widgetPrivils[vecs[j].name] = vecs[j].state;
                    }
                }
            }

            ret = true;
            break;
        }
    }

    return true;
}
