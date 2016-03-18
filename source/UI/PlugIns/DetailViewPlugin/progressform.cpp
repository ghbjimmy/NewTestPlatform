#include "progressform.h"
#include <QHBoxLayout>
#include <QLabel>

ProgressForm::ProgressForm(QWidget *parent) : QScrollArea(parent)
{
    setupUI();
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

ProgressForm::~ProgressForm()
{

}

QHBoxLayout* createLayout(const QString& name, QProgressBar* bar)
{
    QHBoxLayout* h1 = new QHBoxLayout();
    QLabel* lbl = new QLabel();
    lbl->setText(name);
    lbl->setAlignment(Qt::AlignRight);
    lbl->setFixedWidth(45);
    h1->addWidget(lbl);
    h1->addWidget(bar, 1);
    h1->setSpacing(10);

    return h1;
}

void ProgressForm::setupUI()
{
    for (int i = 0; i < SEQ_NUM; ++i)
    {
        _bar[i] = new QProgressBar();
    }

    QVBoxLayout* v1 = new QVBoxLayout();
    v1->addSpacerItem(new QSpacerItem(10,10, QSizePolicy::Minimum, QSizePolicy::Minimum));
    v1->addLayout(createLayout("UUT1:", _bar[0]));
    v1->addLayout(createLayout("UUT2:", _bar[1]));
    v1->addLayout(createLayout("UUT3:", _bar[2]));
    v1->addLayout(createLayout("UUT4:", _bar[3]));
    v1->addLayout(createLayout("UUT5:", _bar[4]));
    v1->addLayout(createLayout("UUT6:", _bar[5]));
    v1->addStretch(1);
    v1->setSpacing(25);

    this->setLayout(v1);
}

void ProgressForm::setBarMaxSize(int size)
{
    for (int i = 0; i < SEQ_NUM; ++i)
    {
        _bar[i]->setRange(0, size);
    }
}

void ProgressForm::setBarValue(int index, int value)
{

}
