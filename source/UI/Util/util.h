#ifndef UTIL_H
#define UTIL_H

#include <QString>
#include <QPalette>
#include <QWidget>

class UIUtil
{
public:
    static void setColor(QPalette::ColorRole acr, const QColor &acolor, QWidget* wgt);

private:
    UIUtil(){}

};

#endif
