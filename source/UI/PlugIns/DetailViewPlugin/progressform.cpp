#include "progressform.h"

#include <QProgressBar>
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
    QVBoxLayout* v1 = new QVBoxLayout();
    v1->addSpacerItem(new QSpacerItem(10,10, QSizePolicy::Minimum, QSizePolicy::Minimum));
    for (int i = 0; i < SEQ_NUM; ++i)
    {
        _bar[i] = new QProgressBar();
        QString name = "UUT" + QString::number(i + 1) + ":";
        v1->addLayout(createLayout(name, _bar[i]));
    }

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
    if (_bar[index]->maximum() < value)
        return;
    _bar[index]->setValue(value);
}

void ProgressForm::increaseBarValue(int index)
{
    int curVal = _bar[index]->value();
    if (curVal == -1)
        curVal = 0;
    if (_bar[index]->maximum() >= curVal)
    {
        _bar[index]->setValue(curVal + 1);
    }
}
