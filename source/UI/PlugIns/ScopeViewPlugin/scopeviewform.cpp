#include "scopeviewform.h"
#include <QPalette>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QGridLayout>

#include "../common/uutbutton.h"
#include "../../Util/util.h"
#include "../../Util/message.h"


ScopeviewForm::ScopeviewForm(IPlugin* plugIn, QWidget *parent) : QWidget(parent)
{
    _plugIn = plugIn;
    setupUI();
    //this->setWidgetResizable(true);
    //this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
}

ScopeviewForm::~ScopeviewForm()
{

}

QGroupBox* createGroupBox(int num)
{
    QGroupBox* gbox = new QGroupBox();
    gbox->setTitle("Plate" + QString::number(num));
    QGridLayout* gridLayout = new QGridLayout();

    int height = 30;
    UutButton* btn1 = new UutButton(1);
    btn1->setText("UUT1\nReady");
    btn1->setFixedHeight(height);

    UutButton* btn2 = new UutButton(1);
    btn2->setText("UUT2\nReady");
    btn2->setFixedHeight(height);

    UutButton* btn3 = new UutButton(1);
    btn3->setText("UUT3\nReady");
    btn3->setFixedHeight(height);

    UutButton* btn4 = new UutButton(1);
    btn4->setText("UUT4\nReady");
    btn4->setFixedHeight(height);

    UutButton* btn5 = new UutButton(1);
    btn5->setText("UUT5\nReady");
    btn5->setFixedHeight(height);

    UutButton* btn6 = new UutButton(1);
    btn6->setText("UUT6\nReady");
    btn6->setFixedHeight(height);

    gridLayout->addWidget(btn1,0,0,1,1);
    gridLayout->addWidget(btn2,0,1,1,1);
    gridLayout->addWidget(btn3,1,0,1,1);
    gridLayout->addWidget(btn4,1,1,1,1);
    gridLayout->addWidget(btn5,2,0,1,1);
    gridLayout->addWidget(btn6,2,1,1,1);

    gbox->setLayout(gridLayout);
    return gbox;
}

QWidget* createPanelWgt()
{

    QWidget* panelWgt = new QWidget();
    QGridLayout* g1 = new QGridLayout();

    for (int i = 0; i < 4; ++i)
    {
        QGroupBox* gbox = createGroupBox(i + 1);
        g1->addWidget(gbox, 0, i, 1, 1);

        gbox->setFixedSize(220, 140);
    }

    panelWgt->setLayout(g1);
    UIUtil::setBgColor(panelWgt, Qt::gray);
    return panelWgt;
}

void ScopeviewForm::setupUI()
{
    QVBoxLayout* v1 = new QVBoxLayout();
    QPushButton* btn = new QPushButton();
    btn->setText("SendTest");
    connect(btn, SIGNAL(clicked()), this, SLOT(onClicked()));
    //v1->addWidget(btn);
    v1->addWidget(createPanelWgt(), 1);
    //v1->addWidget(createStatusWgt());
    this->setLayout(v1);
}

void ScopeviewForm::onBtnCheckBoxStatedChanged(int state)
{

}

void ScopeviewForm::onClicked()
{
    ChannelStateMsg msg;
    _plugIn->sendMessage(&msg);
}
