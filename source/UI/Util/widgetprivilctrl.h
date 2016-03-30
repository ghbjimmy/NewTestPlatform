#ifndef WIDGETPRIVILCTRL_H
#define WIDGETPRIVILCTRL_H

#include <QVector>
#include <QMap>

class QObject;

//控件权限管理类
class WidgetPrivilCtrl
{
public:
    WidgetPrivilCtrl();
    ~WidgetPrivilCtrl();

    void addWidget(QObject* obj);
    void removeWidget(QObject* obj);

    void onWidgetPrivilCtrl(const QMap<QString, int>& widgetPrivils);

    inline bool isNeedPrivil() {return _isNeedPrivil;}

    //清除权限控制约束
    void clearPrivil();

private:
    QVector<QObject*> _needPrivilCtrls;
    bool _isNeedPrivil;
};

#endif // WIDGETPRIVILCTRL_H
