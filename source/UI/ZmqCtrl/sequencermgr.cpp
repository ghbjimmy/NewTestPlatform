#include "sequencermgr.h"
#include "zmqcfgparser.h"
#include "sequencerrpc.h"
#include "qlog.h"
#include "util.h"

SequencerMgr::SequencerMgr() : QObject()
{

}

SequencerMgr::~SequencerMgr()
{
    for (int i = 0; i < SEQ_NUM; ++i)
    {
        delete _sequencers[i];
        _sequencers[i] = NULL;
    }
}

bool SequencerMgr::initByCfg(ZmqCfgParser* cfg)
{
    int baseReqPort = cfg->getSeqPort();
    int basePubPort = cfg->getSeqPub();

    for (int i = 0; i < SEQ_NUM; ++i)
    {
        _sequencers[i] = new SequencerRpc(i);
        if (!_sequencers[i]->init(cfg->getSeqIp(), basePubPort + i,
                                 cfg->getSeqIp(), baseReqPort + i))
        {
            LogMsg(Error, "SequencerMgr::initByCfg failed.");
            return false;
        }

        connect(_sequencers[i], SIGNAL(isAliveSignal(int,bool,bool)), this, SIGNAL(isAliveSignal(int,bool,bool)));
        connect(_sequencers[i], SIGNAL(eventSignal(int,int,const QString&)), this, SIGNAL(eventSignal(int,int,const QString&)));
    }

    return true;
}

bool SequencerMgr::startAll()
{
    for (int i = 0; i < SEQ_NUM; ++i)
    {
        _sequencers[i]->start();
    }

    return true;
}

void SequencerMgr::stopAll()
{
    for (int i = 0; i < SEQ_NUM; ++i)
    {
        _sequencers[i]->stop();
    }
}

QVector<int> SequencerMgr::loadProfile(const QString& csvFilePath)
{
    QVector<int> failVecs;
    for (int i = 0; i < SEQ_NUM; ++i)
    {

        emit showLoadingInfoSignal(QString("%1  Sequence %2 start loading...").arg(UIUtil::getNowTime()).arg(i), 0);
        if (_sequencers[i]->getAliveState() == Dead)
        {
            continue;
        }

        
        if (!_sequencers[i]->loadProfile(csvFilePath))
        {
            LogMsg(Error, "sequencers[%d] load loadProfile failed.", i);
            failVecs.push_back(i);

            emit showLoadingInfoSignal(QString("%1  Sequence %2 load file failed").arg(UIUtil::getNowTime()).arg(i), -1);
            continue;
        }

        emit showLoadingInfoSignal(QString("%1  Sequence %2 load file success").arg(UIUtil::getNowTime()).arg(i), 1);
    }

    emit loadCompleteSignal();

    return failVecs;
}


bool SequencerMgr::getCsvContent(QVector<QString>& items)
{
    //目前只需从第一个活的那里获取结果就行
    for (int i = 0; i < SEQ_NUM; ++i)
    {
        if (_sequencers[i]->getAliveState() != Alive)
            continue;

        return _sequencers[i]->getCsvContent(items);
    }

    return false;
}

int SequencerMgr::getSeqSize()
{
    return SEQ_NUM;
}
