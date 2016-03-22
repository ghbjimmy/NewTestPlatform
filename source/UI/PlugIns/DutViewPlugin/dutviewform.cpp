#include "dutviewform.h"
#include "qlog.h"
#include "dutviewconfigdlg.h"
#include "dutzmqrpc.h"
#include <QTabWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QToolBar>
#include <QToolButton>
#include <QListWidget>
#include <QFile>
#include <QDomDocument>


static const QString& DUT_CONFIG = "dutconfig.xml";
DutViewForm::DutViewForm(IPlugin* plugIn, QWidget *parent) : QWidget(parent)
{
    _plugIn = plugIn;
    setupUI();
}

DutViewForm::~DutViewForm()
{

}

QToolBar* DutViewForm::createToolBar()
{
    QToolBar* bar = new QToolBar();
    QToolButton* tBtn = new QToolButton();
    connect(tBtn, SIGNAL(clicked()), this, SLOT(onConfig()));
    tBtn->setText("Config");
    bar->addWidget(tBtn);

    _tStartBtn = new QToolButton();
    connect(_tStartBtn, SIGNAL(clicked()), this, SLOT(onStart()));
    _tStartBtn->setText("Start");
    bar->addWidget(_tStartBtn);

    _tStopBtn = new QToolButton();
    _tStopBtn->setEnabled(false);
    connect(_tStopBtn, SIGNAL(clicked()), this, SLOT(onStop()));
    _tStopBtn->setText("Stop");
    bar->addWidget(_tStopBtn);

    return bar;
}

void DutViewForm::setupUI()
{ 
    QToolBar* bar = createToolBar();
    QComboBox* cbx = new QComboBox();

    QListWidget* list_widget = new QListWidget();
    cbx->setModel(list_widget->model());
    cbx->setView(list_widget);
    cbx->setEditable(true);

    for (int i = 0; i < 6; i++)
    {
        list_widget->addItem("QQ" + QString::number(i));
    }


    QPushButton* btn = new QPushButton();
    btn->setText("Send");
    QHBoxLayout* h1 = new QHBoxLayout();
    h1->addWidget(cbx, 1);
    h1->addWidget(btn);
    h1->setContentsMargins(0,0,0,0);

    _tabWgt = new QTabWidget();

    QVBoxLayout* v1 = new QVBoxLayout();
    v1->addWidget(bar);
    v1->addWidget(_tabWgt, 1);
    v1->addLayout(h1);
    v1->setContentsMargins(3,3,3,3);

    this->setLayout(v1);

    _cfgDlg = new DutViewConfigDlg(this);
}

bool DutViewForm::LoadCfg(const QString& path)
{
    bool ret = _cfgDlg->loadData(path);
    if (ret)
    {
        const QVector<TSlotConfig>& datas = _cfgDlg->getData();
        for (int i = 0; i < datas.size(); ++i)
        {
            QListWidget* listwgt = new QListWidget();
            _tabWgt->addTab(listwgt, "UUT" + QString::number(i+1));
        }
    }

    return ret;
}

void DutViewForm::onConfig()
{
    _cfgDlg->exec();
}

void DutViewForm::onStart()
{
    const QVector<TSlotConfig>& datas = _cfgDlg->getData();
    int size = datas.size();
    for (int i = 0; i < size; ++i)
    {
        DutZmqRpc* rpc = new DutZmqRpc(i);
        if (!rpc->init(datas[i].subIp, datas[i].subPort, datas[i].reqIp, datas[i].reqPort))
        {
            LogMsg(Error, "DutZmq init failed.");
            continue;
        }

        rpc->start();
        dutRpcs.push_back(rpc);
    }

    _tStartBtn->setEnabled(false);
    _tStopBtn->setEnabled(true);
}

void DutViewForm::onStop()
{
    foreach (DutZmqRpc* rpc, dutRpcs)
    {
        rpc->stop();
        delete rpc;
    }

    dutRpcs.clear();

    _tStartBtn->setEnabled(true);
    _tStopBtn->setEnabled(false);
}
