#include "testenginerpc.h"
#include "zmqsocket.h"
#include "qlog.h"
#include "command.h"

#include <thread>

static void sub_recvData(void* obj)
{
    TestEngineRpc* engRpc = (TestEngineRpc*)obj;
    int timeoutNum = 0;
    int sendHeartBeatNum = 0;
    while(!engRpc->isStop())
    {
        ZmqSocket* subSocket = engRpc->getSubSocket();
        if (subSocket->select(ZMQ_POLLIN, 500) == 0)
        {
            timeoutNum++;
            if (timeoutNum >= 18)
            {
                engRpc->setAlive(false);
                timeoutNum = 0;
            }
        }
        else
        {
            Buffer rcvBuff;
            int cnt = subSocket->recvData(rcvBuff);
            QString recvMsg = QString::fromLocal8Bit(rcvBuff.getBuf(), rcvBuff.getLen());
            if (recvMsg.contains("FCT_HEARTBEAT"))
            {
                engRpc->setAlive(true);
                timeoutNum = 0;
            }
        }

        sendHeartBeatNum++;
        if (sendHeartBeatNum == 4)
        {
            engRpc->aliveNoity(true);
        }
        else if (sendHeartBeatNum == 5)
        {
            engRpc->aliveNoity(false);
            sendHeartBeatNum = 0;
        }
    }
}

TestEngineRpc::TestEngineRpc(int index)
{
    _index = index;
    _subSocket = NULL;
    _subThread = NULL;
    _aliveFlag = 0;
    _isStop = false;
}

TestEngineRpc::~TestEngineRpc()
{

}

bool TestEngineRpc::init(const QString &pubIp, int pubPort)
{
    _subSocket = new ZmqSocket(ZMQ_SUB);
    if (!_subSocket->connect(pubIp.toStdString().c_str(), pubPort))
    {
        LogMsg(Error, "connet sequencer pub failed. ip:%s port %d", pubIp.toStdString().c_str(), pubPort);
        return false;
    }

    _subSocket->setSockOpt(ZMQ_SUBSCRIBE, "", 0);

    return true;
}

bool TestEngineRpc::start()
{
    _subThread = new std::thread(sub_recvData, this);
    return true;
}

void TestEngineRpc::stop()
{
    _isStop = true;
    _subThread->join();
}

void TestEngineRpc::setAlive(bool flag)
{
    _aliveFlag = (flag ? 1 : 2);
}

bool TestEngineRpc::isAlive()
{
    return _aliveFlag == 1 ? true : false;
}

void TestEngineRpc::aliveNoity(bool isShow)
{
    if (_aliveFlag != 0)
        emit isAliveSignal(_index, isAlive(), isShow);
}
