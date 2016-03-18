#include "testenginemgr.h"
#include "zmqcfgparser.h"
#include "testenginerpc.h"
#include "qlog.h"

#include <thread>

TestEngineMgr::TestEngineMgr()
{

}

TestEngineMgr::~TestEngineMgr()
{
    for (int i = 0; i < ENG_NUM; ++i)
    {
        delete _engines[i];
        _engines[i] = NULL;
    }
}

bool TestEngineMgr::initByCfg(ZmqCfgParser* cfg)
{
    int basePubPort = cfg->getTestEnginePub();

    for (int i = 0; i < ENG_NUM; ++i)
    {
        _engines[i] = new TestEngineRpc(i);
        if (!_engines[i]->init(cfg->getTestEngineIp(), basePubPort + i, "", -1))
        {
            LogMsg(Error, "TestEngineMgr::initByCfg failed.");
            return false;
        }

        connect(_engines[i], SIGNAL(isAliveSignal(int,bool,bool)), this, SIGNAL(isAliveSignal(int,bool,bool)));
    }

    return true;
}

bool TestEngineMgr::startAll()
{
    for (int i = 0; i < ENG_NUM; ++i)
    {
        _engines[i]->start();
    }

    return true;
}

void TestEngineMgr::stopAll()
{
    for (int i = 0; i < ENG_NUM; ++i)
    {
        _engines[i]->stop();
    }
}
