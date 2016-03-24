#include "startloaderform.h"

#include <QLabel>
#include <QListWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>

static void startFun(void* obj)
{
    StartLoaderForm* form = (StartLoaderForm*)obj;
    form->run();
}

StartLoaderForm::StartLoaderForm(int& isFinish, QWidget* parent) : _isFinish(isFinish), QWidget(parent)
{
    setupUI();
    this->setWindowFlags(Qt::FramelessWindowHint);
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
    QListWidget* wgt = new QListWidget();
    wgt->setMinimumHeight(200);
    QVBoxLayout* v1 = new QVBoxLayout();
    v1->addWidget(imgLbl);
    v1->addWidget(wgt);
    v1->setContentsMargins(1,1,1,1);
    this->setLayout(v1);
}

bool StartLoaderForm::start()
{
    _thread = new std::thread(startFun, this);

    return true;
}

bool StartLoaderForm::run()
{
    for (int i = 0; i < 15; ++i)
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

    _isFinish = 1;

    return true;
}
