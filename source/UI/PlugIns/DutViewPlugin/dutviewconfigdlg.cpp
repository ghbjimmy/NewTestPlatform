#include "dutviewconfigdlg.h"
#include "qlog.h"

#include <QTableWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDomDocument>
#include <QFile>

static const QString& DUT_CONFIG = "dutconfig.xml";

DutViewConfigDlg::DutViewConfigDlg(QWidget *parent) : QDialog(parent)
{
    setupUI();
}

DutViewConfigDlg::~DutViewConfigDlg()
{

}

void DutViewConfigDlg::setupUI()
{
    _cfgTbl = new QTableWidget();
    _cfgTbl->setColumnCount(5);

    QStringList header;
    header << "Index" << "IP(Req)" << "Port(Req)" << "IP(Sub)" << "Port(Sub)";
    _cfgTbl->setHorizontalHeaderLabels(header);

    _cfgTbl->setColumnWidth(0, 60);
    _cfgTbl->setColumnWidth(2, 80);
    _cfgTbl->setColumnWidth(4, 80);
    QPushButton* okBtn = new QPushButton();
    okBtn->setText("OK");
    QPushButton* cancelBtn = new QPushButton();
    cancelBtn->setText("Cancel");

    QHBoxLayout* h1 = new QHBoxLayout();
    h1->addStretch(1);
    h1->addWidget(okBtn);
    h1->addWidget(cancelBtn);

    connect(okBtn, SIGNAL(clicked()), this, SLOT(onOK()));
    connect(cancelBtn, SIGNAL(clicked()), this, SLOT(onCancel()));

    QVBoxLayout* v1 = new QVBoxLayout();
    v1->addWidget(_cfgTbl, 1);
    v1->addLayout(h1);

    this->setLayout(v1);

    this->setFixedWidth(480);
    this->setMinimumHeight(360);
    this->setWindowTitle("DutConfig");

}

void DutViewConfigDlg::setData(const QVector<TSlotConfig>& cfgs)
{
    _cfgTbl->clear();
    int rowCnt = cfgs.size();
    _cfgTbl->setRowCount(rowCnt);

    for (int i = 0; i < rowCnt; ++i)
    {
        const TSlotConfig& slot = cfgs.at(i);
        _cfgTbl->setItem(i,0,new QTableWidgetItem(QString::number(slot.index)));
        _cfgTbl->setItem(i,1,new QTableWidgetItem(slot.reqIp));
        _cfgTbl->setItem(i,2,new QTableWidgetItem(QString::number(slot.reqPort)));
        _cfgTbl->setItem(i,3,new QTableWidgetItem(slot.subIp));
        _cfgTbl->setItem(i,4,new QTableWidgetItem(QString::number(slot.subPort)));
    }
}

bool DutViewConfigDlg::loadData(const QString& path)
{
    _slotCfgs.clear();
    QString file_name = path + "/config/" + DUT_CONFIG;
    QFile file(file_name);
    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        LogMsg(Error, "open file failed. %s", file_name.toStdString().c_str());
        return false;
    }

    _dom = new QDomDocument();
    QString error;
    int row = 0, column = 0;
    if(!_dom->setContent(&file, false, &error, &row, &column))
    {
        LogMsg(Error, "open xml file failed1. %s", file_name.toStdString().c_str());
        return false;
    }

    file.close();
    if(_dom->isNull())
    {
        LogMsg(Error, "open xml file failed. %s", file_name.toStdString().c_str());
        return false;
    }

    QDomElement root = _dom->documentElement();
    QDomNodeList slotNodes =  root.childNodes();
    for (int i = 0; i < slotNodes.size(); ++i)
    {
        TSlotConfig cfg;
        cfg.index = i;
        QDomNodeList addrNodes = slotNodes.at(i).childNodes();
        for (int j = 0; j < addrNodes.size(); ++j)
        {
            QDomElement node = addrNodes.at(j).toElement();
            if (node.tagName() == "Req")
            {
                cfg.reqIp = node.attribute("Ip");
                cfg.reqPort = node.attribute("Port").toInt();
            }
            else if (node.tagName() == "Sub")
            {
                cfg.subIp = node.attribute("Ip");
                cfg.subPort = node.attribute("Port").toInt();
            }
        }

        _slotCfgs.push_back(cfg);
    }

    setData(_slotCfgs);
    return true;
}

void DutViewConfigDlg::onOK()
{
    //更新内容
    int rowCnt = _cfgTbl->rowCount();
    for (int i = 0; i < rowCnt; ++i)
    {
        _slotCfgs[i].index = _cfgTbl->item(i, 0)->text().toInt();
        _slotCfgs[i].reqIp = _cfgTbl->item(i, 1)->text();
        _slotCfgs[i].reqPort = _cfgTbl->item(i, 2)->text().toInt();
        _slotCfgs[i].subIp = _cfgTbl->item(i, 3)->text();
        _slotCfgs[i].subPort = _cfgTbl->item(i, 4)->text().toInt();
    }

    //保存xml
    /*QDomElement root = _dom->documentElement();
    QDomNodeList slotNodes =  root.childNodes();
    for (int i = 0; i < slotNodes.size(); ++i)
    {
        TSlotConfig cfg;
        cfg.index = i;
        QDomNodeList addrNodes = slotNodes.at(i).childNodes();
        for (int j = 0; j < addrNodes.size(); ++j)
        {
            QDomElement node = addrNodes.at(j).toElement();
            if (node.tagName() == "Req")
            {
                node.at
            }
            else if (node.tagName() == "Sub")
            {
                cfg.subIp = node.attribute("Ip");
                cfg.subPort = node.attribute("Port").toInt();
            }
        }

        _slotCfgs.push_back(cfg);
    }*/

    this->close();
}

void DutViewConfigDlg::onCancel()
{
    this->reject();
}
