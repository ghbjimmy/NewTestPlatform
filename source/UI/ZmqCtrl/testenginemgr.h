#ifndef TESTENGINEMGR_H
#define TESTENGINEMGR_H

#include <QObject>
#include "const.h"

class TestEngineRpc;
class ZmqCfgParser;

class TestEngineMgr : public QObject
{
    Q_OBJECT

public:
    TestEngineMgr();
    ~TestEngineMgr();

    bool initByCfg(ZmqCfgParser* cfg);
    bool startAll();
    void stopAll();

signals:
    void isAliveSignal(int id, bool isAlive, bool isShow);

private:
    TestEngineRpc* _engines[ENG_NUM];
};

#endif // TESTENGINEMGR_H
