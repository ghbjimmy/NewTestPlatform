#include "util.h"

#include <QHBoxLayout>
#include <QDateTime>
#include <QDesktopWidget>
#include <QApplication>
#include <QSize>
#include <QAction>
#include <QCryptographicHash>

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

void UIUtil::moveToScreenCenter(QWidget* wgt)
{
    QDesktopWidget *desk = QApplication::desktop();
    int wd = desk->width();
    int ht = desk->height();
    wgt->move((wd - wgt->width()) / 2,(ht - wgt->height()) / 2);
}

QString UIUtil::EncryptMd5(const QString& str)
{
    QByteArray hash_byte_array = QCryptographicHash::hash(str.toLatin1(), QCryptographicHash::Md5);
    return hash_byte_array.toHex();
}

QString UIUtil::Decrypt(const QString& str)
{
    return QString();
}

