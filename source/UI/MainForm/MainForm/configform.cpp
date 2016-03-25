#include "configform.h"
#include "util.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>
#include <QRadioButton>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QGridLayout>
#include <QComboBox>

ConfigForm::ConfigForm(QWidget *parent) : QDialog(parent)
{
    setupUI();
}

ConfigForm::~ConfigForm()
{

}

QCheckBox* createCheckBox(const QString& text)
{
    QCheckBox* cb = new QCheckBox();
    cb->setText(text);
    return cb;
}

QRadioButton* createRBtn(const QString& text)
{
    QRadioButton* rbtn = new QRadioButton();
    rbtn->setText(text);
    return rbtn;
}

QPushButton* createPushButton(const QString& text)
{
    QPushButton* btn = new QPushButton();
    btn->setText(text);
    return btn;
}

QBoxLayout* ConfigForm::createApplLyt()
{
    QLabel* applbl = new QLabel("Application");
    _scanBarCodeCb = createCheckBox("Scan BarCode");
    _checkEEEECb = createCheckBox("Check EEEECode");
    _scanCfgCb = createCheckBox("Scan CFG");
    _puddingPdcaCb = createCheckBox("Pudding to PDCA");
    _queryResuCb = createCheckBox("Query Previous Stations Test Result");

    _resultLdt = new QLineEdit();

    QHBoxLayout* h1 = new QHBoxLayout();
    h1->addWidget(_scanBarCodeCb);
    h1->addWidget(_checkEEEECb);

    QHBoxLayout* h2 = new QHBoxLayout();
    h2->addStretch(1);
    h2->addWidget(_resultLdt);
    h2->addStretch(1);

    QWidget* bgWgt = new QWidget();
    UIUtil::setBgColor(bgWgt, QColor(195,195,195));
    QVBoxLayout* v1 = new QVBoxLayout();
    v1->addLayout(h1);
    v1->addWidget(_scanCfgCb);
    v1->addWidget(_puddingPdcaCb);
    v1->addWidget(_queryResuCb);
    v1->addLayout(h2);
    bgWgt->setLayout(v1);

    QVBoxLayout* v2 = new QVBoxLayout();
    v2->addWidget(applbl);
    v2->addWidget(bgWgt);
    return v2;
}

QBoxLayout* ConfigForm::createTrigLyt()
{
    QLabel* triglbl = new QLabel("Trigger");

    _manualRbtn = createRBtn("Manual");
    _extRbtn = createRBtn("External");
    _bothRbtn = createRBtn("Both");
    _scanRbtn = createRBtn("Scan");

    _startLdt = new QLineEdit();
    _startLdt->setText("START");
    _autoModeCb = createCheckBox("Autonmation Mode");

    QHBoxLayout* h1 = new QHBoxLayout();
    h1->addWidget(_scanRbtn);
    h1->addWidget(_startLdt);

    QVBoxLayout* v1 = new QVBoxLayout();
    v1->addWidget(_manualRbtn);
    v1->addWidget(_extRbtn);
    v1->addWidget(_bothRbtn);
    v1->addLayout(h1);
    QWidget* bgWgt = new QWidget();
    UIUtil::setBgColor(bgWgt, QColor(195,195,195));
    bgWgt->setLayout(v1);

    QVBoxLayout* v2 = new QVBoxLayout();
    v2->addWidget(triglbl);
    v2->addWidget(bgWgt);
    v2->addWidget(_autoModeCb);
    v2->addStretch();

    return v2;
}

QWidget* ConfigForm::createGeneralTab()
{
    QHBoxLayout* h1 = new QHBoxLayout();
    h1->addLayout(createApplLyt());
    h1->addStretch(1);
    h1->addLayout(createTrigLyt());

    QLabel* fixtuLbl = new QLabel("Fixture ID:");
    _fixtureLdt = new QLineEdit();
    _fixtureLdt->setMinimumWidth(60);
    QHBoxLayout* h2 = new QHBoxLayout();
    h2->addWidget(fixtuLbl);
    h2->addWidget(_fixtureLdt);
    h2->addStretch(1);

    QLabel* fileLbl = new QLabel("File:");

    QLabel* pathLbl = new QLabel("Log Path:");
    _fileEdit = new QLineEdit();
    _broweBtn = createPushButton("Browe");

    QHBoxLayout* h3 = new QHBoxLayout();
    h3->addWidget(pathLbl);
    h3->addWidget(_fileEdit,1);
    h3->addWidget(_broweBtn);

    QWidget* bgWgt = new QWidget();
    UIUtil::setBgColor(bgWgt, QColor(195,195,195));
    bgWgt->setLayout(h3);

    QVBoxLayout* v1 = new QVBoxLayout();
    v1->addWidget(fileLbl);
    v1->addWidget(bgWgt);

    _okBtn = createPushButton("OK");
    _cancelBtn = createPushButton("Cancel");

    QVBoxLayout* v2 = new QVBoxLayout();
    v2->addLayout(h1,1);
    v2->addLayout(h2);
    v2->addLayout(v1);
    v2->addStretch(1);

    QWidget* wgt = new QWidget();
    wgt->setLayout(v2);
    return wgt;
}


QBoxLayout* ConfigForm::createSNFormatLyt()
{
    QLabel* lbl = new QLabel("SN formate:");
    QLabel* lbl1 = new QLabel("MLB:");
    _mlbLdt = new QLineEdit();
    _mlbLdt->setMinimumWidth(100);
    QLabel* lbl2 = new QLabel("CFG:");
    _cfgLdt = new QLineEdit();
    _cfgLdt->setMinimumWidth(100);

    QGridLayout* g1 = new QGridLayout();
    g1->addWidget(lbl1, 0, 0);
    g1->addWidget(_mlbLdt, 0, 1);
    g1->addWidget(lbl2, 1, 0);
    g1->addWidget(_cfgLdt, 1, 1);

    QVBoxLayout* v2 = new QVBoxLayout();
    v2->addLayout(g1);
    v2->addStretch(1);

    QWidget* bgWgt = new QWidget();
    UIUtil::setBgColor(bgWgt, QColor(195,195,195));
    bgWgt->setLayout(v2);
    bgWgt->setMinimumWidth(150);
    QVBoxLayout* v1 = new QVBoxLayout();
    v1->addWidget(lbl);
    v1->addWidget(bgWgt);

    return v1;
}

QBoxLayout* ConfigForm::createPubBLogLyt()
{
    QLabel* lbl = new QLabel("Pudding blob:");
    _logCb = createCheckBox("Uart Log");
    _flowCb = createCheckBox("Test Flow");
    _removeCb = createCheckBox("Remove Local Blob");
    _removeCb->setEnabled(false);
    _puddingCbx = new QComboBox();
    _puddingCbx->setMinimumWidth(120);

    QVBoxLayout* v1 = new QVBoxLayout();
    v1->addWidget(_puddingCbx);
    v1->addWidget(_logCb);
    v1->addWidget(_flowCb);
    v1->addWidget(_removeCb);

    QHBoxLayout* h1 = new QHBoxLayout();
    h1->addLayout(v1);
    h1->addStretch(1);

    QWidget* bgWgt = new QWidget();
    UIUtil::setBgColor(bgWgt, QColor(195,195,195));
    bgWgt->setLayout(h1);
    bgWgt->setMinimumWidth(200);

    QVBoxLayout* v2 = new QVBoxLayout();
    v2->addWidget(lbl);
    v2->addWidget(bgWgt);

    return v2;

}

QWidget* ConfigForm::createAdvanceTab()
{
    QLabel* lbl = new QLabel("Stop test when failed terms more than:(-1:will continue test all item");
    _stopValLdt = new QLineEdit();
    _stopValLdt->setText("-1");
    _stopValLdt->setMaximumWidth(60);

    QVBoxLayout* v1 = new QVBoxLayout();
    v1->addWidget(lbl);
    v1->addWidget(_stopValLdt);

    QLabel* lbl1 = new QLabel("Max reset times per Unit");
    _maxResetLdt = new QLineEdit();
    _maxResetLdt->setText("0");
    _maxResetLdt->setMaximumWidth(60);

    QVBoxLayout* v2 = new QVBoxLayout();
    v2->addWidget(lbl1);
    v2->addWidget(_maxResetLdt);

    QHBoxLayout* h1 = new QHBoxLayout();
    h1->addLayout(createSNFormatLyt());
    h1->addStretch(1);
    h1->addLayout(createPubBLogLyt());
    h1->addStretch(2);

    QVBoxLayout* v3 = new QVBoxLayout();
    v3->addLayout(v1);
    v3->addLayout(v2);
    v3->addLayout(h1);
    v3->addStretch(1);

    QWidget* wgt = new QWidget();
    wgt->setLayout(v3);

    return wgt;
}

void ConfigForm::setupUI()
{
    QTabWidget* tab = new QTabWidget();
    tab->addTab(createGeneralTab(), "General");
    tab->addTab(createAdvanceTab(), "Advance");
    tab->addTab(new QWidget(), "About");

    QHBoxLayout* h1= new QHBoxLayout();
    h1->addStretch(3);
    h1->addWidget(_okBtn);
    h1->addStretch(1);
    h1->addWidget(_cancelBtn);
    h1->addStretch(3);

    QVBoxLayout* v1 = new QVBoxLayout();
    v1->addWidget(tab,1);
    v1 ->addLayout(h1);

    this->setLayout(v1);
    this->setFixedSize(600, 350);
}
