#ifndef USERCTRL_H
#define USERCTRL_H

#include <QObject>
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

    bool parseConfig(const QString& path);

    bool checkUser(const QString& name, const QString& pwd);


private:
    QVector<TUserPrivilege> _userPris;
    QMap<int, QVector<TWidgetPrivilege> > _wgtPrivileges;
};

#endif // USERCTRL_H
