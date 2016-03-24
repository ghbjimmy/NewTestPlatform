#include "baserpc.h"
#include "zmqsocket.h"
#include "qlog.h"
#include "command.h"
#include "const.h"

QString getRecvData(const QString& recvMsg)
{
    QStringList list = recvMsg.split("!@#");
    if (list.size() != 5)
        return QString();

    return list[4];
}

static void sub_recvData(void* obj)
{
    BaseRpc* rpc = (BaseRpc*)obj;
    int timeoutNum = 0;
    int sendHeartBeatNum = 0;
    while(!rpc->isStop())
    {
        ZmqSocket* subSocket = rpc->getSubSocket();
        if (subSocket == NULL)
        {
            LogMsg(Error, "sub socket is null, can not procss sub msg.");
            return;
        }

        if (subSocket->select(ZMQ_POLLIN, 500) == 0)
        {
            timeoutNum++;
            if (timeoutNum >= 18)
            {
                rpc->setAlive(false);
                rpc->aliveNoity(false);
                timeoutNum = 0;
            }
        }
        else
        {
            Buffer rcvBuff;
            int cnt = subSocket->recvData(rcvBuff);
            QString recvMsg = QString::fromLocal8Bit(rcvBuff.getBuf(), rcvBuff.getLen());
            QString data = getRecvData(recvMsg);
            if (data == "FCT_HEARTBEAT")
            {
                rpc->setAlive(true);
                rpc->aliveNoity(true);
                timeoutNum = 0;
            }
            else
            {
                rpc->procSubRecvMsg(data);
            }
        }

        //控制心跳指示灯闪
        sendHeartBeatNum++;
        if (sendHeartBeatNum == 4)
        {
            rpc->aliveNoity(true);
        }
        else if (sendHeartBeatNum == 5)
        {
            rpc->aliveNoity(false);
            sendHeartBeatNum = 0;
        }
    }
}

BaseRpc::BaseRpc(int index)
{
    _index = index;
    _subSocket = NULL;
    _reqSocket = NULL;
    _subThread = NULL;
    _aliveState = unknown;
    _isStop = false;
}

BaseRpc::~BaseRpc()
{
    if (_reqSocket != NULL)
    {
        delete _reqSocket;
        _reqSocket = NULL;
    }

    if (_subSocket != NULL)
    {
        delete _subSocket;
        _subSocket = NULL;
    }

    if (_subThread != NULL)
    {
        delete _subThread;
        _subThread = NULL;
    }
}

bool BaseRpc::init(const QString&  pubIp, int pubPort, const QString&  reqIp, int reqPort)
{
    if (!pubIp.isEmpty() && pubPort > 0)
    {
        _subSocket = new ZmqSocket(ZMQ_SUB);
        if (!_subSocket->connect(pubIp.toStdString().c_str(), pubPort))
        {
            LogMsg(Error, "connet sequencer pub failed. ip:%s port %d", pubIp.toStdString().c_str(), pubPort);
            return false;
        }

        _subSocket->setSockOpt(ZMQ_SUBSCRIBE, (void*)"", 0);
    }

    if (!reqIp.isEmpty() && reqPort > 0)
    {
        _reqSocket = new ZmqSocket(ZMQ_REQ);
        if (!_reqSocket->connect(reqIp.toStdString().c_str(), reqPort))
        {
            LogMsg(Error, "connet sequencer req failed. ip:%s port %d", reqIp.toStdString().c_str(), reqPort);
            return false;
        }

        _reqSocket->setSockOpt(ZMQ_RCVTIMEO, (void*)&TIME_OUT, sizeof(int));
        _reqSocket->setSockOpt(ZMQ_SNDTIMEO, (void*)&TIME_OUT, sizeof(int));
    }

    return true;
}

bool BaseRpc::start()
{
    _subThread = new std::thread(sub_recvData, this);
    return true;
}

void BaseRpc::stop()
{
    _isStop = true;
    _subThread->join();
}

void BaseRpc::setAlive(bool flag)
{
    _aliveState = flag ? Alive : Dead;
}

ERpcState BaseRpc::getAliveState()
{
    return _aliveState;
}


void BaseRpc::aliveNoity(bool isShow)
{
    if (_aliveState != unknown)
    {
        bool flag = _aliveState == Alive ? true : false;
        emit isAliveSignal(_index, flag, isShow);
    }
}

bool BaseRpc::procSubRecvMsg(const QString& msg)
{
    return true;
}
