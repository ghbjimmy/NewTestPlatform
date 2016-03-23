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
#include <QLineEdit>
#include <QSpacerItem>
#include <QMessageBox>

static const QString& DUT_CONFIG = "dutconfig.xml";
DutViewForm::DutViewForm(IPlugin* plugIn, QWidget *parent) : QWidget(parent)
{
    _isAutoScroll = true;
    _plugIn = plugIn;
    setupUI();
}

DutViewForm::~DutViewForm()
{

}

QToolBar* DutViewForm::createToolBar()
{
    QToolBar* bar = new QToolBar();
    _tConfigBtn = new QToolButton();
    connect(_tConfigBtn, SIGNAL(clicked()), this, SLOT(onConfig()));
    _tConfigBtn->setText("Config");
    bar->addWidget(_tConfigBtn);

    _tStartBtn = new QToolButton();
    connect(_tStartBtn, SIGNAL(clicked()), this, SLOT(onStart()));
    _tStartBtn->setText("Start");
    bar->addWidget(_tStartBtn);

    _tStopBtn = new QToolButton();
    _tStopBtn->setEnabled(false);
    connect(_tStopBtn, SIGNAL(clicked()), this, SLOT(onStop()));
    _tStopBtn->setText("Stop");
    bar->addWidget(_tStopBtn);


    _tClearMsgBtn = new QToolButton();
    connect(_tClearMsgBtn, SIGNAL(clicked()), this, SLOT(onClear()));
    _tClearMsgBtn->setText("Clear");
    bar->addWidget(_tClearMsgBtn);

    _tSelAutoScrollBtn = new QToolButton();
    connect(_tSelAutoScrollBtn, SIGNAL(clicked()), this, SLOT(onScroll()));
    _tSelAutoScrollBtn->setText("AutoScroll");
    bar->addWidget(_tSelAutoScrollBtn);
    return bar;
}

void DutViewForm::setupUI()
{ 
    QToolBar* bar = createToolBar();
    _cbx = new QComboBox();
    _cbxListWgt = new QListWidget();
    _cbx->setModel(_cbxListWgt->model());
    _cbx->setView(_cbxListWgt);
    _cbx->setEditable(true);

    _sendBtn = new QPushButton();
    _sendBtn->setText("Send");
    _sendBtn->setEnabled(false);
    connect(_sendBtn, SIGNAL(clicked()), this, SLOT(onSend()));

    QHBoxLayout* h1 = new QHBoxLayout();
    h1->addWidget(_cbx, 1);
    h1->addWidget(_sendBtn);
    h1->setContentsMargins(0,0,0,0);

    _tabWgt = new QTabWidget();

    _recvDataEdit = new QLineEdit();
    QHBoxLayout* h2 = new QHBoxLayout();
    h2->addWidget(_recvDataEdit, 1);
    h2->addSpacerItem(new QSpacerItem(80,10, QSizePolicy::Expanding, QSizePolicy::Minimum));
    h2->setContentsMargins(0,0,0,0);

    QVBoxLayout* v1 = new QVBoxLayout();
    v1->addWidget(bar);
    v1->addWidget(_tabWgt, 1);
    v1->addLayout(h1);
    v1->addLayout(h2);
    //v1->setContentsMargins(3,3,3,3);

    this->setLayout(v1);

    _cfgDlg = new DutViewConfigDlg(this);
}

bool DutViewForm::LoadCfg(const QString& path)
{
    bool ret = _cfgDlg->loadData(path);
    if (ret)
    {
        const QVector<TSlotConfig>& datas = _cfgDlg->getData();
        int size = datas.size();
        for (int i = 0; i < size; ++i)
        {
            QListWidget* listwgt = new QListWidget();
            _tabWgt->addTab(listwgt, "UUT" + QString::number(i+1));
            _msgList.push_back(listwgt);
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

        connect(rpc, SIGNAL(dutMsgSignal(int, const QString&)), this, SLOT(onDutRecvMsg(int,const QString&)));
        rpc->start();
        dutRpcs.push_back(rpc);
    }

    _tConfigBtn->setEnabled(false);
    _tStartBtn->setEnabled(false);
    _tStopBtn->setEnabled(true);
    _sendBtn->setEnabled(true);
}

void DutViewForm::onStop()
{
    foreach (DutZmqRpc* rpc, dutRpcs)
    {
        rpc->stop();
        delete rpc;
    }

    dutRpcs.clear();

    _tConfigBtn->setEnabled(true);
    _tStartBtn->setEnabled(true);
    _tStopBtn->setEnabled(false);
    _sendBtn->setEnabled(false);
}

void DutViewForm::onClear()
{
    int size = _msgList.size();
    for (int i = 0; i < size; ++i)
    {
        _msgList[i]->clear();
    }
}

void DutViewForm::onScroll()
{
    _isAutoScroll = !_isAutoScroll;

    _tSelAutoScrollBtn->setText(_isAutoScroll ? "AutoScroll" : "NoScroll");
}

void DutViewForm::onSend()
{
    QString curCmd = _cbx->currentText();
    if (curCmd.isEmpty())
    {
        QMessageBox::warning(this, "Send Data", "Please Input Command Data.");
        return;
    }

    int index = _tabWgt->currentIndex();
    DutZmqRpc* curRpc = dutRpcs[index];

    QString recvData;
    if (!curRpc->sendCommand(curCmd,recvData))
    {
        _recvDataEdit->setText("error:recvOverTime.");
    }
    else
    {
        _recvDataEdit->setText(recvData);

        bool existFlag = false;
        int cnt = _cbxListWgt->count();
        for (int i = 0; i < cnt; ++i)
        {
            if (curCmd == _cbxListWgt->item(i)->text())
            {
                existFlag = true;
                break;
            }
        }
        if (!existFlag)
        {
            _cbxListWgt->addItem(curCmd);
            if (cnt > 10)
            {
                QListWidgetItem* item = _cbxListWgt->takeItem(0);
                _cbxListWgt->removeItemWidget(item);
            }
        }
    }
}

void DutViewForm::onDutRecvMsg(int index, const QString& msg)
{
    if (index < _msgList.size())
    {
        _msgList[index]->addItem(msg);
        if (_isAutoScroll)
        {
            int size = _msgList[index]->count();
            _msgList[index]->setCurrentRow(size - 1);
        }
    }
}
