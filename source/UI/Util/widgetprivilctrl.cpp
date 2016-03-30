#include "widgetprivilctrl.h"

#include <QWidget>
#include <QAction>

WidgetPrivilCtrl::WidgetPrivilCtrl()
{
    _isNeedPrivil = false;
}

WidgetPrivilCtrl::~WidgetPrivilCtrl()
{

}

void WidgetPrivilCtrl::addWidget(QObject* obj)
{
    _needPrivilCtrls.push_back(obj);
}

void WidgetPrivilCtrl::removeWidget(QObject* obj)
{
    if (_needPrivilCtrls.contains(obj))
        _needPrivilCtrls.removeOne(obj);
}

void setState(QObject* obj, int state)
{
    QWidget* wgt = dynamic_cast<QWidget*>(obj);
    if (NULL != wgt)
    {
        if (state == 0)
        {
            wgt->hide();
        }
        else if (state == 1)
        {
            if (wgt->isHidden())
                wgt->show();

            wgt->setEnabled(false);
        }
        else if (state == -1)//去掉权限控制
        {
            if (wgt->isHidden())
                wgt->show();

            wgt->setEnabled(true);
        }
    }
    else
    {
        QAction* act = dynamic_cast<QAction*>(obj);
        if (state == 0)
        {
            act->setEnabled(false);
        }
        else if (state == 1)
        {
            act->setEnabled(false);
        }
        else if (state == -1)//去掉权限控制
        {

            act->setEnabled(true);
        }
    }
}

void WidgetPrivilCtrl::onWidgetPrivilCtrl(const QMap<QString, int>& widgetPrivils)
{
    if (widgetPrivils.empty())
    {
        _isNeedPrivil = false;
        return;
    }

    int size = _needPrivilCtrls.size();
    auto iter = widgetPrivils.begin();
    for (; iter != widgetPrivils.end(); ++iter)
    {
        for (int i = 0; i < size; ++i)
        {
            QObject* obj = _needPrivilCtrls[i];
            if (obj->objectName() == iter.key())
            {//0:隐藏 1:可见只读

                setState(obj, iter.value());
                _isNeedPrivil = true;
            }
        }
    }
}

void WidgetPrivilCtrl::clearPrivil()
{
    _isNeedPrivil = false;
    int size = _needPrivilCtrls.size();
    for (int i = 0; i < size; ++i)
    {
        QObject* obj = _needPrivilCtrls[i];
        setState(obj, -1);
    }
}
