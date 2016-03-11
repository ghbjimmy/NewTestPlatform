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
    _bar1 = new QProgressBar();
    _bar2 = new QProgressBar();
    _bar3 = new QProgressBar();
    _bar4 = new QProgressBar();
    _bar5 = new QProgressBar();
    _bar6 = new QProgressBar();

    QVBoxLayout* v1 = new QVBoxLayout();
    v1->addSpacerItem(new QSpacerItem(10,10, QSizePolicy::Minimum, QSizePolicy::Minimum));
    v1->addLayout(createLayout("UUT1:", _bar1));
    v1->addLayout(createLayout("UUT2:", _bar2));
    v1->addLayout(createLayout("UUT3:", _bar3));
    v1->addLayout(createLayout("UUT4:", _bar4));
    v1->addLayout(createLayout("UUT5:", _bar5));
    v1->addLayout(createLayout("UUT6:", _bar6));
    v1->addStretch(1);
    v1->setSpacing(25);

    this->setLayout(v1);
}
