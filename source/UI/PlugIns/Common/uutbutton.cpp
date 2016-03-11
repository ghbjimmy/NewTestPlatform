#include "uutbutton.h"
#include <QCheckBox>
#include <QLabel>
#include <QPalette>
#include <QPainter>
#include "../../Util/util.h"

UutButton::UutButton(int style, QWidget *parent) : QPushButton(parent)
{
    _index = 1;
    _style = style;
    setupUI();
}

UutButton::~UutButton()
{

}

void UutButton::setupUI()
{
    _box = new QCheckBox(this);
    _box->setFixedSize(14,14);
    connect(_box, SIGNAL(stateChanged(int)), this, SIGNAL(signal_check(int)));


    _txtLbl = new QLabel(this);
    _txtLbl->setText(QString::number(_index));
    _txtLbl->setFixedSize(14,14);

    _txtLbl->setAlignment(Qt::AlignCenter);
    _box->move(3, this->height() / 2 - _box->height() / 2);
    _txtLbl->move(this->width() - 10, 3);

    if (_style == 0)
    {
        QString btn_style = "QPushButton{background:rgb(0,128,255)}";
        this->setStyleSheet(btn_style);
        QString label_style="QLabel{background-color:yellow;}";
        _txtLbl->setStyleSheet(label_style);
    }
    else
    {
        //QString btn_style = "QPushButton{background:rgb(178,216,253);font-size:12px}";
        QString btn_style = "QPushButton{background:rgb(0,128,255);font-size:12px}";
        this->setStyleSheet(btn_style);
        _txtLbl->hide();
    }
}

void UutButton::setIndex(int index)
{
    _index = index;
    _txtLbl->setText(QString::number(_index));
}

void UutButton::resizeEvent(QResizeEvent* evt)
{
    _box->move(3, this->height() / 2 - _box->height() / 2);
    _txtLbl->move(this->width() - _txtLbl->width() - 2, 3);
}

bool UutButton::isChecked()
{
    return _box->isChecked();
}

void UutButton::setChecked(bool flag)
{
    _box->setChecked(flag);
}

/*void UutButton::onStateChanged(int state)
{
    emit signal_check(state);
}*/
