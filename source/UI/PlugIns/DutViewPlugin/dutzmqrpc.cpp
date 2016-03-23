#include "dutzmqrpc.h"
#include "zmqsocket.h"
#include "qlog.h"
#include "command.h"
#include "const.h"

#include <thread>


static void sub_recvData(void* obj)
{
    DutZmqRpc* rpc = (DutZmqRpc*)obj;
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
            continue;
        }
        else
        {
            Buffer rcvBuff;
            int cnt = subSocket->recvData(rcvBuff);
            QString recvMsg = QString::fromLocal8Bit(rcvBuff.getBuf(), rcvBuff.getLen());
            rpc->procSubRecvMsg(recvMsg);
        }
    }

}

DutZmqRpc::DutZmqRpc(int index)
{
    _index = index;
    _subSocket = NULL;
    _reqSocket = NULL;
    _subThread = NULL;
    _isStop = false;
}

DutZmqRpc::~DutZmqRpc()
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

bool DutZmqRpc::init(const QString&  pubIp, int pubPort, const QString&  reqIp, int reqPort)
{
    if (!pubIp.isEmpty() && pubPort > 0)
    {
        _subSocket = new ZmqSocket(ZMQ_SUB);
        if (!_subSocket->connect(pubIp.toStdString().c_str(), pubPort))
        {
            LogMsg(Error, "connet sequencer pub failed. ip:%s port %d", pubIp.toStdString().c_str(), pubPort);
            return false;
        }

        _subSocket->setSockOpt(ZMQ_SUBSCRIBE, "", 0);
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

bool DutZmqRpc::start()
{
    _subThread = new std::thread(sub_recvData, this);
    return true;
}

void DutZmqRpc::stop()
{
    _isStop = true;
    _subThread->join();
}

bool DutZmqRpc::procSubRecvMsg(const QString& msg)
{
    emit dutMsgSignal(_index, msg);
    return true;
}

bool DutZmqRpc::sendCommand(const QString& sendMsg, QString& recvMsg)
{
    int len = _reqSocket->sendData(sendMsg.toStdString().c_str(), sendMsg.length());
    if (len < 0)
        return false;

    if (_reqSocket->select(ZMQ_POLLIN, TIME_OUT) == 0)
    {
        LogMsg(Error, "sendCommand failed. error is : recv data over time.");
        return false;
    }

    Buffer recvbuf;
    if (_reqSocket->recvData(recvbuf) < 0)
    {
        LogMsg(Error, "sendCommand failed. error is : recv data failed.");
        return false;
    }

    QByteArray array(recvbuf.getBuf(), recvbuf.getLen());
    recvMsg = QString(array);

    return true;
}
