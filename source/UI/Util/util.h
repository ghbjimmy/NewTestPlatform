#ifndef UTIL_H
#define UTIL_H

#include <QString>
#include <QPalette>
#include <QWidget>

class QLayout;

class UIUtil
{
public:
    static void setBgColor(QWidget* wgt, const QColor &color);

    static void setFontSize(QWidget* wgt, int size = -1, bool isBold = false);
    static void setTextColor(QWidget* wgt, const QColor& color);

private:
    UIUtil(){}

};

#endif
