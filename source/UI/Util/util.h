#ifndef UTIL_H
#define UTIL_H

#include <QString>
#include <QPalette>
#include <QWidget>


class UIUtil
{
public:
    static void setBgColor(QWidget* wgt, const QColor &color);

    static void setFontSize(QWidget* wgt, int size = -1, bool isBold = false);
    static void setTextColor(QWidget* wgt, const QColor& color);

    static QString getNowTime();//获取当前时间 精确到毫秒

    static void moveToScreenCenter(QWidget* wgt); //移到屏幕中央

    static QString EncryptMd5(const QString& str);
    static QString Decrypt(const QString& str);

    //创建widget及子类，并可设置名字
    template <class T>
    static T* createWidgetWithName(const QString& text, const QString& objName="")
    {
        T* obj = new T(0);
        obj->setText(text);
        if (!objName.isEmpty())
            obj->setObjectName(objName);

        return obj;
    }

private:
    UIUtil(){}

};

#endif
