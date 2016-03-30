#ifndef USERCTRL_H
#define USERCTRL_H

#include <QVector>
#include <QMap>

struct TUserPrivilege
{
    QString name;
    QString password;
    int level; //用户级别： 0:admin 1:engineer 2:operator
};

struct TWidgetPrivilege //控件权限
{
    QString name;
    int level;
    int state; //0:隐藏 1:可见只读
};

//用户权限管理
class UserCtrl
{

public:
    UserCtrl();
    ~UserCtrl();

    bool init();

    bool login(const QString& name, const QString& pwd, QMap<QString, int>& widgetPrivils);
    bool logout();



private:
    bool parseConfig(const QString& path);

private:
    QVector<TUserPrivilege> _userPris;
    QMap<int, QVector<TWidgetPrivilege> > _wgtPrivileges;

    int _curUserIndex; //当前用户的index
};

#endif // USERCTRL_H
