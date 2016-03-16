#ifndef SEQUENCERMGR_H
#define SEQUENCERMGR_H


class SequencerRpc;
class ZmqCfgParser;
//sequencer 管理器
class SequencerMgr
{
public:
    SequencerMgr();
    ~SequencerMgr();

    bool initByCfg(ZmqCfgParser* cfg);
    bool start();

private:
    SequencerRpc* _sequencers[6];
};

#endif // SEQUENCERMGR_H
