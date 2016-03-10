#include "scopeviewform.h"
#include <QPalette>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

ScopeviewForm::ScopeviewForm(QWidget *parent) : QWidget(parent)
{
    setupUI();
}

ScopeviewForm::~ScopeviewForm()
{

}

QWidget* createPanelWgt()
{
    QWidget* panelWgt = new QWidget();
    QPalette palette;
    palette.setColor(QPalette::Background, Qt::gray);
    panelWgt->setPalette(palette);
    panelWgt->setAutoFillBackground(true);
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
