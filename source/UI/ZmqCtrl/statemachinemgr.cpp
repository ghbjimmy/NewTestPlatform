#include "statemachinerpc.h"
#include "statemachinemgr.h"
#include "zmqcfgparser.h"
#include "qlog.h"

#include <thread>

StateMachineMgr::StateMachineMgr()
{

}

StateMachineMgr::~StateMachineMgr()
{
    for (int i = 0; i < SM_NUM; ++i)
    {
        delete _sms[i];
        _sms[i] = NULL;
    }
}

bool StateMachineMgr::initByCfg(ZmqCfgParser* cfg)
{
    int basePubPort = cfg->getSmHeartBeat();
    int baseReqPort = cfg->getSmPort();
    for (int i = 0; i < SM_NUM; ++i)
    {
        _sms[i] = new StateMachineRpc(i);
        if (!_sms[i]->init(cfg->getSmIp(), basePubPort + i,
                           cfg->getSmIp(), baseReqPort + i))
        {
            LogMsg(Error, "StateMachineMgr::initByCfg failed.");
            return false;
        }

        connect(_sms[i], SIGNAL(isAliveSignal(int,bool,bool)), this, SIGNAL(isAliveSignal(int,bool,bool)));
    }

    return true;
}

bool StateMachineMgr::startAll()
{
    for (int i = 0; i < SM_NUM; ++i)
    {
        _sms[i]->start();
    }

    return true;
}

void StateMachineMgr::stopAll()
{
    for (int i = 0; i < SM_NUM; ++i)
    {
        _sms[i]->stop();
    }
}

bool StateMachineMgr::startTest()
{
    bool ret = false;
    for (int i = 0; i < SM_NUM; ++i)
    {
        ret = _sms[i]->startTest();
    }

    return ret;
}

bool StateMachineMgr::stopTest()
{
    bool ret = false;
    for (int i = 0; i < SM_NUM; ++i)
    {
        ret = _sms[i]->stopTest();
    }

    return ret;
}
