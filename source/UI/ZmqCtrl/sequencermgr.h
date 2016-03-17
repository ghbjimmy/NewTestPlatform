#ifndef SEQUENCERMGR_H
#define SEQUENCERMGR_H

#include <QVector>

class SequencerRpc;
class ZmqCfgParser;

const int SEQ_NUM = 1;
//sequencer 管理器
class SequencerMgr
{
public:
    SequencerMgr();
    ~SequencerMgr();

    bool initByCfg(ZmqCfgParser* cfg);
    bool startAll();
    void stopAll();

    //加载csv
    QVector<int> loadProfile(const QString& csvFilePath);

    //获取csv内存
    bool getContent(QVector<QString>& items);

private:
    SequencerRpc* _sequencers[SEQ_NUM];
};

#endif // SEQUENCERMGR_H
