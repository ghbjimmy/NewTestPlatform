#include "dutviewform.h"
#include "qlog.h"
#include "dutviewconfigdlg.h"
#include "dutzmqrpc.h"
#include "util.h"

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
#include <QSplitter>
#include <QTextEdit>
#include <QList>
#include <QAction>
#include <QApplication>

static const QString& DUT_PANNEL = "Dut Pannel";
static const QString& DUT_CONFIG = "dutconfig.xml";
DutViewForm::DutViewForm(IPlugin* plugIn, QWidget *parent) : IModuleForm(plugIn, parent)
{
    setupUI(); 
}

DutViewForm::~DutViewForm()
{

}

bool DutViewForm::init()
{
    QString runPath = QApplication::applicationDirPath();
    if (!LoadCfg(runPath))
    {
        return false;
    }

    return true;
}

void DutViewForm::clear()
{

}

QVector<QAction*> DutViewForm::getActions()
{
    return QVector<QAction*>() << _showFormAct;
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

    return bar;
}

QWidget* DutViewForm::createSendWidget()
{
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
    h1->addWidget(_sendBtn);
    h1->addStretch(1);

    QVBoxLayout* v1 = new QVBoxLayout();
    v1->addLayout(h1);
    v1->addWidget(_cbx);
    v1->addStretch(1);
    v1->setContentsMargins(0,0,0,0);

    QWidget* bgWgt = new QWidget();
    bgWgt->setLayout(v1);
    //bgWgt->setStyleSheet ("background-color: rgb(0,0,0);color: rgb(255,255,255);");

    QVBoxLayout* v2 = new QVBoxLayout();
    v2->addWidget(bgWgt);
    v2->addStretch(1);
    v2->setContentsMargins(0,0,0,0);

    QWidget* wgt = new QWidget();
    wgt->setLayout(v1);
    wgt->setMaximumWidth(160);

    return wgt;
}

QWidget* DutViewForm::createTextEdit()
{
    _clearTxtBtn = new QPushButton();
    _clearTxtBtn->setText("Clear");
    connect(_clearTxtBtn, SIGNAL(clicked()), this, SLOT(onClearCmd()));

    QHBoxLayout* h1 = new QHBoxLayout();
    h1->addStretch(1);
    h1->addWidget(_clearTxtBtn);

    _recvDataEdit = new QTextEdit();
    _recvDataEdit->setReadOnly(true);

    QVBoxLayout* v1 = new QVBoxLayout();
    v1->addLayout(h1);
    v1->addWidget(_recvDataEdit, 1);
    v1->setContentsMargins(0,0,0,0);

    QWidget* wgt = new QWidget();
    wgt->setLayout(v1);
    return wgt;
}

void DutViewForm::setupUI()
{ 
    _showFormAct = UIUtil::createWidgetWithName<QAction>(DUT_PANNEL);
    connect(_showFormAct, SIGNAL(triggered()), this, SLOT(onShowForm()));

    QToolBar* bar = createToolBar();
    _tabWgt = new QTabWidget();

    QVBoxLayout* v1 = new QVBoxLayout();
    v1->addWidget(bar);
    v1->addWidget(_tabWgt, 1);
    v1->setContentsMargins(3,3,3,3);

    QWidget* topWgt = new QWidget();
    topWgt->setLayout(v1);


    QWidget* sendWgt = createSendWidget();

    QWidget* textWgt = createTextEdit();
    QHBoxLayout* h2 = new QHBoxLayout();
    h2->addWidget(sendWgt);
    h2->addWidget(textWgt);
    h2->setContentsMargins(3,3,3,3);

    QWidget* bottomWgt = new QWidget();
    bottomWgt->setLayout(h2);


    QSplitter* split = new QSplitter(Qt::Orientation::Vertical);
    split->addWidget(topWgt);
    split->addWidget(bottomWgt);


    split->setStretchFactor(0, 1);
    split->setStretchFactor(1, 0);

    split->setSizes(QList<int>() << 400 << 100);
    QVBoxLayout* v2 = new QVBoxLayout();
    v2->addWidget(split,1);
    this->setLayout(v2);

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
            QTextEdit* textEdit = new QTextEdit();
            _tabWgt->addTab(textEdit, "UUT" + QString::number(i+1));
            _msgList.push_back(textEdit);
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
            onAppendText(QString("Create Client[%1] Failed: tcp://%2:%3 tcp://%4:%5").arg(i).arg(datas[i].subIp)
                         .arg(datas[i].subPort).arg(datas[i].reqIp).arg(datas[i].reqPort), -1);
            continue;
        }

        onAppendText(QString("Create Client[%1] Success: tcp://%2:%3 tcp://%4:%5").arg(i).arg(datas[i].subIp)
                     .arg(datas[i].subPort).arg(datas[i].reqIp).arg(datas[i].reqPort), 1);

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
    int i = 0;
    foreach (DutZmqRpc* rpc, dutRpcs)
    {
        rpc->stop();
        onAppendText(QString("Stop Client[%1] Success").arg(i++), 1);
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
    onAppendText("Req: " + curCmd, 0);
    if (!curRpc->sendCommand(curCmd,recvData))
    {
        onAppendText("Rsp: " + QString("recv data time out."), -1);
    }
    else
    {
        onAppendText("Rsp: " + recvData, 1);

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
        _msgList[index]->append(msg);
    }
}

void DutViewForm::onAppendText(const QString& text, int state)
{
    QString colorText;
    if (state == 0)
        colorText = QString("<font color=black>%1</font>\n").arg(text);
    else if (state == 1)
        colorText = QString("<font color=blue>%1</font>\n").arg(text);
    else
        colorText = QString("<font color=red>%1</font>\n").arg(text);

    _recvDataEdit->append(colorText);
}

void DutViewForm::onClearCmd()
{
    _recvDataEdit->clear();
}

void DutViewForm::onShowForm()
{
    QDialog* dlg = NULL;
    if (_ownerWgt != NULL)
        dlg = new QDialog(_ownerWgt);
    else
        dlg = new QDialog();

    dlg->setWindowTitle("Dut Debug Pannel");
    QHBoxLayout* h1 = new QHBoxLayout();
    h1->addWidget(this);
    h1->setContentsMargins(0,0,0,0);
    dlg->setLayout(h1);
    dlg->resize(800, 640);
    dlg->show();

    //dlg 内存需要处理，暂时先放着
}
