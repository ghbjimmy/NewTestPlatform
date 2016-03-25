#include "loginform.h"
#include "util.h"

#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QSpacerItem>

LoginForm::LoginForm()
{
    setupUI();
}

LoginForm::~LoginForm()
{

}

void LoginForm::setupUI()
{
    QLabel* usrLbl = new QLabel("User:");
    _userLdt = new QLineEdit();
    _userLdt->setFixedWidth(200);
    QLabel* pwdLbl = new QLabel("Password:");
    _pwdLdt = new QLineEdit();
    _pwdLdt->setFixedWidth(200);
    QGridLayout* g1 = new QGridLayout();
    g1->setSpacing(10);

    g1->addWidget(usrLbl, 0, 0);
    g1->addWidget(_userLdt, 0, 1);
    g1->addItem(new QSpacerItem(10,10, QSizePolicy::Expanding, QSizePolicy::Minimum), 0, 2);
    g1->addWidget(pwdLbl, 1, 0);
    g1->addWidget(_pwdLdt, 1, 1);
    g1->addItem(new QSpacerItem(10,10, QSizePolicy::Expanding, QSizePolicy::Minimum), 1, 2);

    QWidget* bgWgt = new QWidget();
    UIUtil::setBgColor(bgWgt, QColor(195,195,195));

    bgWgt->setLayout(g1);

    _okBtn = new QPushButton();
    _okBtn->setText("OK");
    _cancelBtn = new QPushButton();
    _cancelBtn->setText("Cancel");

    QHBoxLayout* h1 = new QHBoxLayout();
    h1->addStretch(3);
    h1->addWidget(_okBtn);
    h1->addStretch(1);
    h1->addWidget(_cancelBtn);
    h1->addStretch(3);

    connect(_okBtn, SIGNAL(onOK()), this, SLOT(onCancel()));
    connect(_cancelBtn, SIGNAL(onOK()), this, SLOT(onCancel()));

    QVBoxLayout* v1 = new QVBoxLayout();
    v1->addWidget(bgWgt);
    v1->addLayout(h1);

    this->setLayout(v1);

    this->setFixedSize(380, 160);
}

void LoginForm::onOK()
{
    //this->close();
}

void LoginForm::onCancel()
{
    this->close();
}
