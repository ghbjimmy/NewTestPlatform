#include "interactionviewform.h"
#include "uutbutton.h"
#include "util.h"

#include <QHBoxLayout>
#include <QProgressBar>
#include <QLabel>
#include <QCheckBox>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QSet>

InteractionViewForm::InteractionViewForm(IPlugin* plugIn, QWidget *parent) : QScrollArea(parent)
{
    _plugIn = plugIn;
    setupUI();
}

InteractionViewForm::~InteractionViewForm()
{

}

QHBoxLayout* InteractionViewForm::createBtnLayout(UutButton*& btn, int index, const QString& text)
{
    int height = 40;
    btn = new UutButton(0);
    btn->setIndex(index);
    btn->setText(text);
    btn->setFixedHeight(height);

    QProgressBar* bar = new QProgressBar();
    QHBoxLayout* h2 = new QHBoxLayout();
    QLabel* tlbl = new QLabel();
    tlbl->setText("000.000");

    QLabel* ulbl = new QLabel();
    ulbl->setText("s");

    h2->addWidget(tlbl);
    h2->addWidget(ulbl);

    QVBoxLayout* v1 = new QVBoxLayout();
    v1->addWidget(bar);
    v1->addLayout(h2);

    QHBoxLayout* h1 = new QHBoxLayout();
    h1->addWidget(btn, 1);
    h1->addLayout(v1);
    h1->setSpacing(6);
    return h1;

}

QWidget* createStationWgt()
{
    QLabel* slbl = new QLabel("Station ID:   ");
    slbl->setAlignment(Qt::AlignRight);

    QLabel* fLbl = new QLabel("FixtureID:   ");
    fLbl->setAlignment(Qt::AlignRight);

    QLabel* nLbl = new QLabel("Line NO.:   ");
    nLbl->setAlignment(Qt::AlignRight);

    QLabel* statIdLbl = new QLabel("Station0");
    QLabel* fixtureIDLbl = new QLabel("Fixture001");
    QLabel* lineNoLbl = new QLabel("Line001");

    UIUtil::setTextColor(statIdLbl, Qt::blue);
    UIUtil::setTextColor(fixtureIDLbl, Qt::blue);
    UIUtil::setTextColor(lineNoLbl, Qt::blue);

    QGridLayout* g1 = new QGridLayout();
    g1->addWidget(slbl,0,0);
    g1->addWidget(statIdLbl, 0, 1);
    g1->addWidget(fLbl,1,0);
    g1->addWidget(fixtureIDLbl, 1, 1);
    g1->addWidget(nLbl,2,0);
    g1->addWidget(lineNoLbl, 2, 1);

    QHBoxLayout* h1 = new QHBoxLayout();
    h1->addLayout(g1);
    h1->addStretch(1);
    QWidget* wgt = new QWidget();
    wgt->setLayout(h1);

    UIUtil::setBgColor(wgt, Qt::gray);
    return wgt;
}

QWidget* createSNWgt()
{
    QLabel* snLbl = new QLabel("SN:");
    QLineEdit* snEdit = new QLineEdit();
    QVBoxLayout* v1 = new QVBoxLayout();
    v1->addWidget(snLbl);
    v1->addWidget(snEdit);
    v1->setContentsMargins(QMargins(0,0,0,0));

    QWidget* wgt = new QWidget();
    wgt->setLayout(v1);

    return wgt;
}

QWidget* createtResultWgt()
{
    QLabel* lbl1 = new QLabel("Tested:");
    QLabel* lbl2 = new QLabel("  Rate:");
    QLabel* lbl3 = new QLabel("PASS:");
    QLabel* lbl4 = new QLabel("FAIL:");

    UIUtil::setFontSize(lbl1, -1, true);
    UIUtil::setFontSize(lbl2, -1, true);
    UIUtil::setFontSize(lbl3, -1, true);
    UIUtil::setFontSize(lbl4, -1, true);

    QLabel* passTlbl = new QLabel("0");
    passTlbl->setAlignment(Qt::AlignCenter);
    QLabel* passRlbl = new QLabel("0%");
    passRlbl->setAlignment(Qt::AlignCenter);
    UIUtil::setTextColor(passRlbl, Qt::green);

    QLabel* failTlbl = new QLabel("0");
    failTlbl->setAlignment(Qt::AlignCenter);
    QLabel* failRlbl = new QLabel("0%");
    failRlbl->setAlignment(Qt::AlignCenter);
    UIUtil::setTextColor(failRlbl, Qt::red);


    QGridLayout* g1 = new QGridLayout();
    g1->setSpacing(6);
    g1->addWidget(lbl1, 0, 1);
    g1->addWidget(lbl2, 0, 2);
    g1->addWidget(lbl3, 1, 0);
    g1->addWidget(passTlbl, 1, 1);
    g1->addWidget(passRlbl, 1, 2);
    g1->addWidget(lbl4, 2, 0);
    g1->addWidget(failTlbl, 2, 1);
    g1->addWidget(failRlbl, 2, 2);

    QPushButton* btn = new QPushButton();
    btn->setText("F");
    btn->setMaximumWidth(20);
    QVBoxLayout* v1 = new QVBoxLayout();
    v1->addStretch(1);
    v1->addWidget(btn);

    QHBoxLayout* h1 = new QHBoxLayout();
    h1->addLayout(g1);
    h1->addStretch(1);
    h1->addLayout(v1);
    h1->addSpacerItem(new QSpacerItem(20,10));

    QWidget* wgt = new QWidget();
    wgt->setLayout(h1);
    UIUtil::setBgColor(wgt, Qt::gray);

    wgt->setMinimumSize(200, 80);
    return wgt;
}

QHBoxLayout* createCtrlWgt()
{
    QPushButton* startBtn = new QPushButton();
    startBtn->setText("Start(F5)");

    QPushButton* stopBtn = new QPushButton();
    stopBtn->setText("Stop(F6)");

    QCheckBox* dbgBox = new QCheckBox();
    dbgBox->setEnabled(false);
    dbgBox->setText("Debug(COF)");

    QHBoxLayout* h1 = new QHBoxLayout();
    h1->addWidget(startBtn);
    h1->addStretch(1);
    h1->addWidget(stopBtn);
    h1->addWidget(dbgBox);
    h1->setSpacing(10);
    h1->setContentsMargins(QMargins(0,0,0,0));
    return h1;
}

QHBoxLayout* createUserWgt()
{
    QLabel* lbl = new QLabel("Label");
    QLabel* usrlbl = new QLabel("Name:  ");
    QLabel* namelbl = new QLabel("None");

    QHBoxLayout* h1 = new QHBoxLayout();
    h1->addWidget(lbl);
    h1->addStretch(1);
    h1->addWidget(usrlbl);
    h1->addWidget(namelbl);
    h1->addSpacerItem(new QSpacerItem(20,10));
    h1->setContentsMargins(0,0,0,0);

    return h1;
}

void InteractionViewForm::setupUI()
{
    QHBoxLayout* h1 = createBtnLayout(_btn1, 1, "UUT1\nReady");
    QHBoxLayout* h2 = createBtnLayout(_btn2, 2, "UUT2\nReady");
    QHBoxLayout* h3 = createBtnLayout(_btn3, 3, "UUT3\nReady");
    QHBoxLayout* h4 = createBtnLayout(_btn4, 4, "UUT4\nReady");
    QHBoxLayout* h5 = createBtnLayout(_btn5, 5, "UUT5\nReady");
    QHBoxLayout* h6 = createBtnLayout(_btn6, 6, "UUT6\nReady");

    connect(_btn1, SIGNAL(signal_check(int)), this, SLOT(onBtnCheckBoxStatedChanged(int)));
    connect(_btn2, SIGNAL(signal_check(int)), this, SLOT(onBtnCheckBoxStatedChanged(int)));
    connect(_btn3, SIGNAL(signal_check(int)), this, SLOT(onBtnCheckBoxStatedChanged(int)));
    connect(_btn4, SIGNAL(signal_check(int)), this, SLOT(onBtnCheckBoxStatedChanged(int)));
    connect(_btn5, SIGNAL(signal_check(int)), this, SLOT(onBtnCheckBoxStatedChanged(int)));
    connect(_btn6, SIGNAL(signal_check(int)), this, SLOT(onBtnCheckBoxStatedChanged(int)));

    QVBoxLayout* v2 = new QVBoxLayout();

    v2->addLayout(h1);
    v2->addLayout(h2);
    v2->addLayout(h3);
    v2->addLayout(h4);
    v2->addLayout(h5);
    v2->addLayout(h6);
    v2->setSpacing(0);

    QVBoxLayout* v1 = new QVBoxLayout();
    v1->addLayout(v2);
    _selBox = new QCheckBox();
    _selBox->setText("Select/Unselect All");
    connect(_selBox, SIGNAL(stateChanged(int)), this, SLOT(onSelCheckStateChanged(int)));
    QHBoxLayout* hh1 = new QHBoxLayout();
    hh1->addStretch(1);
    hh1->addWidget(_selBox);
    hh1->addSpacerItem(new QSpacerItem(20,10));
    hh1->addStretch(1);

    v1->addLayout(hh1);
    v1->addStretch(1);

    QVBoxLayout* v3 = new QVBoxLayout();
    v3->addWidget(createStationWgt());
    v3->addWidget(createSNWgt());
    v3->addWidget(createtResultWgt());

    QHBoxLayout* hh2 = new QHBoxLayout();
    hh2->addLayout(v3);
    hh2->addStretch(1);

    v1->addLayout(hh2);

    v1->addLayout(createCtrlWgt());
    v1->addLayout(createUserWgt());
    v1->addSpacerItem(new QSpacerItem(10, 20));
   // v1->addStretch(1);

    v1->setContentsMargins(QMargins(0,10,0,0));
    v1->setSpacing(6);

    QWidget* wgt = new QWidget();
    wgt->setLayout(v1);

    this->setStyleSheet("QScrollArea{border:none}");
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setWidgetResizable(true);
    this->setWidget(wgt);
}

void InteractionViewForm::onBtnCheckBoxStatedChanged(int state)
{
    QSet<int> set;
    set.insert(_btn1->isChecked() ? 1 : 0);
    set.insert(_btn2->isChecked() ? 1 : 0);
    set.insert(_btn3->isChecked() ? 1 : 0);
    set.insert(_btn4->isChecked() ? 1 : 0);
    set.insert(_btn5->isChecked() ? 1 : 0);
    set.insert(_btn6->isChecked() ? 1 : 0);

    disconnect(_selBox, SIGNAL(stateChanged(int)), this, SLOT(onSelCheckStateChanged(int)));
    if (set.size() == 1)
    {
        bool flag = *set.begin() == 1 ? true : false;
        _selBox->setChecked(flag);
    }
    else
        _selBox->setCheckState(Qt::CheckState::PartiallyChecked);

     connect(_selBox, SIGNAL(stateChanged(int)), this, SLOT(onSelCheckStateChanged(int)));
    _selBox->update();
}

 void InteractionViewForm::onSelCheckStateChanged(int state)
 {
     if (state == 1)
     {
         _selBox->setChecked(true);
         return;
     }
     disconnect(_btn1, SIGNAL(signal_check(int)), this, SLOT(onBtnCheckBoxStatedChanged(int)));
     disconnect(_btn2, SIGNAL(signal_check(int)), this, SLOT(onBtnCheckBoxStatedChanged(int)));
     disconnect(_btn3, SIGNAL(signal_check(int)), this, SLOT(onBtnCheckBoxStatedChanged(int)));
     disconnect(_btn4, SIGNAL(signal_check(int)), this, SLOT(onBtnCheckBoxStatedChanged(int)));
     disconnect(_btn5, SIGNAL(signal_check(int)), this, SLOT(onBtnCheckBoxStatedChanged(int)));
     disconnect(_btn6, SIGNAL(signal_check(int)), this, SLOT(onBtnCheckBoxStatedChanged(int)));

     bool flag = state == 0 ? false : true;

     _btn1->setChecked(flag);
     _btn2->setChecked(flag);
     _btn3->setChecked(flag);
     _btn4->setChecked(flag);
     _btn5->setChecked(flag);
     _btn6->setChecked(flag);


     connect(_btn1, SIGNAL(signal_check(int)), this, SLOT(onBtnCheckBoxStatedChanged(int)));
     connect(_btn2, SIGNAL(signal_check(int)), this, SLOT(onBtnCheckBoxStatedChanged(int)));
     connect(_btn3, SIGNAL(signal_check(int)), this, SLOT(onBtnCheckBoxStatedChanged(int)));
     connect(_btn4, SIGNAL(signal_check(int)), this, SLOT(onBtnCheckBoxStatedChanged(int)));
     connect(_btn5, SIGNAL(signal_check(int)), this, SLOT(onBtnCheckBoxStatedChanged(int)));
     connect(_btn6, SIGNAL(signal_check(int)), this, SLOT(onBtnCheckBoxStatedChanged(int)));
 }

 bool InteractionViewForm::onChanelStateMsg(int index, int result)
 {
    return true;
 }
