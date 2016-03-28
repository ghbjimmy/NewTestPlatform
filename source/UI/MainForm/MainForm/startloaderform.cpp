#include "startloaderform.h"
#include "startloadermgr.h"
#include "qlog.h"

#include <QLabel>
#include <QListWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTextEdit>

static void startFun(void* obj)
{
    StartLoaderForm* form = (StartLoaderForm*)obj;
    form->run();
}

StartLoaderForm::StartLoaderForm(int& isFinish, QWidget* parent) : _isFinish(isFinish), QWidget(parent)
{
    setupUI();
    //this->setWindowFlags(Qt::FramelessWindowHint);
    this->setWindowFlags(Qt::CustomizeWindowHint);
}

StartLoaderForm::~StartLoaderForm()
{

}

void StartLoaderForm::setupUI()
{
    QLabel* imgLbl = new QLabel();
    QPixmap pix= QPixmap(":/Resources/start1.png");
    imgLbl->setPixmap(pix);
    imgLbl->setMaximumHeight(480);
    _textEdit = new QTextEdit();
    _textEdit->setReadOnly(true);
    _textEdit->setFixedHeight(180);
    QVBoxLayout* v1 = new QVBoxLayout();
    v1->addWidget(imgLbl, 1);
    v1->addWidget(_textEdit);
    v1->setContentsMargins(1,1,1,1);
    this->setLayout(v1);
}

bool StartLoaderForm::start()
{
    _startLoaderMgr = new StartLoaderMgr();
    if (!_startLoaderMgr->init())
    {
        LogMsg(Error, "StartLoaderMgr init failed.");
        return false;
    }

    connect(_startLoaderMgr, SIGNAL(showStartingInfoSignal(const QString&, int)),
            this, SLOT(onAppendText(const QString&, int)));

    _thread = new std::thread(startFun, this);

    return true;
}

bool StartLoaderForm::run()
{
    if (!_startLoaderMgr->startEng())
    {
        LogMsg(Error, "Start Engine failed.");
        _isFinish = 1;
        return false;
    }

    if (!_startLoaderMgr->startSeq())
    {
        LogMsg(Error, "Start Sequence failed.");
        _isFinish = 1;
        return false;
    }

    if (!_startLoaderMgr->startSM())
    {
        LogMsg(Error, "Start StateMachine failed.");
        _isFinish = 1;
        return false;
    }

    _isFinish = 1;

    return true;
}

void StartLoaderForm::onAppendText(const QString& text, int state)
{
    QString colorText;
    if (state == 0)
        colorText = QString("<font color=black>%1</font>\n").arg(text);
    else if (state == 1)
        colorText = QString("<font color=blue>%1</font>\n").arg(text);
    else
        colorText = QString("<font color=red>%1</font>\n").arg(text);

    _textEdit->append(colorText);
}
