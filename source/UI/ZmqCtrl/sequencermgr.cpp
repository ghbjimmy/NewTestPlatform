#include "sequencermgr.h"
#include "zmqcfgparser.h"
#include "sequencerrpc.h"
#include "qlog.h"

SequencerMgr::SequencerMgr()
{

}

SequencerMgr::~SequencerMgr()
{

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
        if (!_sequencers[i]->loadProfile(csvFilePath))
        {
            LogMsg(Error, "sequencers[%d] load loadProfile failed.", i);
            failVecs.push_back(i);
        }
    }

    return failVecs;
}


bool SequencerMgr::getContent(QVector<QString>& items)
{
    //目前只需从第一个那里获取结果就行
    return _sequencers[0]->getContent(items);
}
