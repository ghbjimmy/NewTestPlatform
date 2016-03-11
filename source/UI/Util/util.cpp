#include "util.h"

void UIUtil::setColor(QPalette::ColorRole acr, const QColor &acolor, QWidget* wgt)
{
    QPalette palette;
    palette.setColor(acr, acolor);
    wgt->setPalette(palette);
    wgt->setAutoFillBackground(true);
}
