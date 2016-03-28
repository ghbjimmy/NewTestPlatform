#include "startloadermgr.h"
#include "util.h"

#include <QThread>

const int sleepTime = 300;
StartLoaderMgr::StartLoaderMgr()
{

}

StartLoaderMgr::~StartLoaderMgr()
{

}

bool StartLoaderMgr::init()
{
    return true;
}

bool StartLoaderMgr::startSeq()
{
    for (int i = 0; i < 6; ++i)
    {
        emit showStartingInfoSignal(QString("%1  Starting Sequence[%2]...").arg(UIUtil::getNowTime()).arg(i), 0);
        QThread::msleep(sleepTime);
        if (i == 3)
            emit showStartingInfoSignal(QString("%1  Start Sequence[%2] failed.").arg(UIUtil::getNowTime()).arg(i), 2);
        else
            emit showStartingInfoSignal(QString("%1  Start Sequence[%2] success.").arg(UIUtil::getNowTime()).arg(i), 1);
    }
    return true;
}

bool StartLoaderMgr::startEng()
{
    for (int i = 0; i < 6; ++i)
    {
        emit showStartingInfoSignal(QString("%1  Starting Engine[%2]...").arg(UIUtil::getNowTime()).arg(i), 0);
        QThread::msleep(sleepTime);
        if (i == 4)
            emit showStartingInfoSignal(QString("%1  Start Engine[%2] failed.").arg(UIUtil::getNowTime()).arg(i), 2);
        else
            emit showStartingInfoSignal(QString("%1  Start Engine[%2] success.").arg(UIUtil::getNowTime()).arg(i), 1);
    }
    return true;
}

bool StartLoaderMgr::startSM()
{
    for (int i = 0; i < 1; ++i)
    {
        emit showStartingInfoSignal(QString("%1  Starting StateMachine[%2]...").arg(UIUtil::getNowTime()).arg(i), 0);
        QThread::msleep(sleepTime);
        emit showStartingInfoSignal(QString("%1  Starting StateMachine[%2] success.").arg(UIUtil::getNowTime()).arg(i), 1);
    }

    return true;
}
