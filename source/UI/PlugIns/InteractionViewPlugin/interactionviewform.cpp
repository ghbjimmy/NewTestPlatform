#include "interactionviewform.h"
#include "uutbutton.h"
#include "util.h"
#include "message.h"
#include "qlog.h"
#include "widgetprivilctrl.h"

#include <QHBoxLayout>
#include <QProgressBar>
#include <QLabel>
#include <QCheckBox>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QSet>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>


InteractionViewForm::InteractionViewForm(IPlugin* plugIn, QWidget *parent) : IModuleForm(plugIn, parent)
{
    _wgtPrivilCtrl = new WidgetPrivilCtrl();
    setupUI();
}

InteractionViewForm::~InteractionViewForm()
{

}

bool InteractionViewForm::init()
{
    return true;
}

void InteractionViewForm::clear()
{
    //_startBtn->setEnabled(true);
    //_stopBtn->setEnabled(false);
}

QVector<QAction*> InteractionViewForm::getActions()
{
    return QVector<QAction*>();
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

    UIUtil::setBgColor(wgt, QColor(195,195,195));
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
    UIUtil::setBgColor(wgt, QColor(195,195,195));

    wgt->setMinimumSize(200, 80);
    return wgt;
}

QHBoxLayout* InteractionViewForm::createCtrlWgt()
{
    _startBtn = UIUtil::createWidgetWithName<QPushButton>("Start(F5)", "Start(F5)");
    connect(_startBtn, SIGNAL(clicked()), this, SLOT(onStart()));

    _stopBtn = UIUtil::createWidgetWithName<QPushButton>("Stop(F6)", "Stop(F6)");
    _stopBtn->setEnabled(false);
    connect(_stopBtn, SIGNAL(clicked()), this, SLOT(onStop()));

    //需要放入权限控制列表
     _wgtPrivilCtrl->addWidget(_stopBtn);

    QCheckBox* dbgBox = new QCheckBox();
    dbgBox->setEnabled(false);
    dbgBox->setText("Debug(COF)");

    QHBoxLayout* h1 = new QHBoxLayout();
    h1->addWidget(_startBtn);
    h1->addStretch(1);
    h1->addWidget(_stopBtn);
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

void InteractionViewForm::connectBtnSignal(UutButton* btn, bool isConnect)
{
    if (isConnect)
        connect(btn, SIGNAL(signal_check(int)), this, SLOT(onBtnCheckBoxStatedChanged(int)));
    else
        disconnect(btn, SIGNAL(signal_check(int)), this, SLOT(onBtnCheckBoxStatedChanged(int)));
}

void InteractionViewForm::setupUI()
{
    QVBoxLayout* v2 = new QVBoxLayout();
    for (int i = 0; i < 6; ++i)
    {
        QString name = "UUT" + QString::number(i + 1) + "\nReady";
        v2->addLayout(createBtnLayout(_btn[i], i, name));
        connectBtnSignal(_btn[i], true);
    }

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


    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidget(wgt);

    scrollArea->setStyleSheet("QScrollArea{border:none}");
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setWidgetResizable(true);


    QVBoxLayout* vv = new QVBoxLayout();
    vv->addWidget(scrollArea);
    vv->setContentsMargins(0,0,0,0);
    this->setLayout(vv);
}

void InteractionViewForm::onBtnCheckBoxStatedChanged(int state)
{
    QSet<int> set;
    for (int i = 0; i < 6; ++i)
    {
        set.insert(_btn[i]->isChecked() ? 1 : 0);
    }

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

    for (int i = 0; i < 6; ++i)
    {
        connectBtnSignal(_btn[0], false);
    }

    bool flag = state == 0 ? false : true;
    for (int i = 0; i < 6; ++i)
    {
        _btn[i]->setChecked(flag);
        connectBtnSignal(_btn[0], true);
    }
}

void InteractionViewForm::onChanelStateMsg(int index, int result)
{
    if (result == 0 || result == -1)
        _btn[index]->showColor(Qt::red);
}

void InteractionViewForm::onSeqStart(int index, const QString& data)
{
    _btn[index]->showColor(Qt::blue);
}

void InteractionViewForm::onSeqEnd(int index, const QString& data)
{
    QJsonParseError json_error;
    QJsonDocument document = QJsonDocument::fromJson(data.toUtf8(), &json_error);
    if(json_error.error != QJsonParseError::NoError)
    {
        LogMsg(Error, "Parse Seq End json failed. %s", data.toStdString().c_str());
        return;
    }

    if (!document.isObject())
    {
        LogMsg(Error, "Parse Seq End json format is error. %s", data.toStdString().c_str());
        return;
    }

    int ret = -9999;
    QJsonObject obj = document.object();
    if (obj.contains("result"))
    {
        int val = -9999;
        if  (-9999 == (val = obj.value("result").toInt(-9999)))
        {
            QString sval = "";
            if ("-9999" == (sval = obj.value("result").toString("-9999")))
            {
                bool bresult = obj.value("result").toBool();
                ret = bresult ? 1 : 0;
            }
            else
            {
                ret = sval.toInt(); // "-1"
            }
        }
        else
        {
            ret = val;
        }
    }

    if (ret == 1)
    {
        _btn[index]->showColor(Qt::green);
    }
    else
    {
        _btn[index]->showColor(Qt::red);
    }

    //测试完需要复位界面
    clear();
}

void InteractionViewForm::onStart()
{
    StartTestMsg msg;
    _plugin->sendMessage(&msg);

    _startBtn->setEnabled(false);

    if (!_wgtPrivilCtrl->isNeedPrivil())
        _stopBtn->setEnabled(true);
}

void InteractionViewForm::onStop()
{
    StopTestMsg msg;
    _plugin->sendMessage(&msg);

    _startBtn->setEnabled(true);
    _stopBtn->setEnabled(false);
}

void InteractionViewForm::onUserLogin(const QMap<QString, int>& userPrivils)
{
    _wgtPrivilCtrl->onWidgetPrivilCtrl(userPrivils);
}
