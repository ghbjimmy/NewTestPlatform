#include "scopeviewform.h"
#include <QPalette>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QGridLayout>

#include "../common/uutbutton.h"
#include "../../Util/util.h"

ScopeviewForm::ScopeviewForm(QWidget *parent) : QWidget(parent)
{
    setupUI();
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


    UIUtil::setColor(QPalette::Background, Qt::gray, panelWgt);
    return panelWgt;
}

QLabel* createNumLabel(int num)
{
    QLabel* numlbl = new QLabel(QString::number(num));
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(0,255,0));
    numlbl->setPalette(palette);

    numlbl->setAlignment(Qt::AlignCenter);
    numlbl->setFixedSize(16, 16);
    numlbl->setAutoFillBackground(true);

    return numlbl;
}

QWidget* createStatusWgt()
{
    QWidget* statusWgt = new QWidget();
    QHBoxLayout* h1 = new QHBoxLayout();
    QLabel* seqLbl = new QLabel("Sequencer:");
    h1->addWidget(seqLbl);

    for (int i = 0; i < 6; ++i)
    {
        h1->addWidget(createNumLabel(i));
    }

    h1->addStretch(1);
    h1->setContentsMargins(QMargins(0,0,0,0));


    QHBoxLayout* h2 = new QHBoxLayout();
    QLabel* engineLbl = new QLabel("Engine:");
    h2->addWidget(engineLbl);
    for (int i = 0; i < 6; ++i)
    {
        h2->addWidget(createNumLabel(i));
    }

    h2->addStretch(1);
    h2->setContentsMargins(QMargins(0,0,0,0));


    QLabel* machineLbl = new QLabel("StateMachine:");
    QLabel* numLbl = createNumLabel(0);

    QHBoxLayout* h3 = new QHBoxLayout();
    h3->addWidget(machineLbl);
    h3->addWidget(numLbl);
    h3->addStretch(1);
    h3->setContentsMargins(QMargins(0,0,0,0));

    QHBoxLayout* h4 = new QHBoxLayout();
    h4->addLayout(h1);
    h4->addSpacerItem(new QSpacerItem(20,10, QSizePolicy::Expanding, QSizePolicy::Expanding));
    h4->addLayout(h2);
    h4->addSpacerItem(new QSpacerItem(20,10, QSizePolicy::Expanding, QSizePolicy::Expanding));
    h4->addLayout(h3);
    h4->addStretch(1);
    h4->setContentsMargins(QMargins(0,0,0,0));

    statusWgt->setLayout(h4);

    return statusWgt;
}

void ScopeviewForm::setupUI()
{
    QVBoxLayout* v1 = new QVBoxLayout();
    v1->addWidget(createPanelWgt(), 1);
    v1->addWidget(createStatusWgt());
    this->setLayout(v1);
}
