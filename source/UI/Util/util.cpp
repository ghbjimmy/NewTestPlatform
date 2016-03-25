#include "util.h"
#include <QHBoxLayout>
#include <QDateTime>

void UIUtil::setBgColor(QWidget* wgt, const QColor &color)
{
    QPalette& palette = const_cast<QPalette&>(wgt->palette());
    palette.setColor(QPalette::Background, color);
    wgt->setAutoFillBackground(true);
}

void UIUtil::setFontSize(QWidget* wgt, int size, bool isBold)
{
    QFont font;
    if (size != -1)
    {
        font.setPointSize(size);
    }
    if (isBold)
    {
        font.setBold(isBold);
        font.setWeight(75);
    }

    wgt->setFont(font);
}

void UIUtil::setTextColor(QWidget* wgt, const QColor& color)
{
    QPalette& palette = const_cast<QPalette&>(wgt->palette());;
    QBrush brush(color);
    brush.setStyle(Qt::SolidPattern);
    palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
    palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
}

QString UIUtil::getNowTime()
{
    return  QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
}
