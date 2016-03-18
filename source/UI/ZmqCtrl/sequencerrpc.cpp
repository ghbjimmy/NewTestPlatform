#include "sequencerrpc.h"
#include "zmqsocket.h"
#include "qlog.h"
#include "command.h"
#include "const.h"

#include <thread>


static void sub_recvData(void* obj)
{
    SequencerRpc* seqRpc = (SequencerRpc*)obj;
    int timeoutNum = 0;
    int sendHeartBeatNum = 0;
    while(!seqRpc->isStop())
    {
        ZmqSocket* subSocket = seqRpc->getSubSocket();
        if (subSocket->select(ZMQ_POLLIN, 500) == 0)
        {
            timeoutNum++;
            if (timeoutNum >= 16)
            {
                seqRpc->setAlive(false);
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
                seqRpc->setAlive(true);
                timeoutNum = 0;
            }
        }

        sendHeartBeatNum++;
        if (sendHeartBeatNum == 4)
        {
            seqRpc->aliveNoity(true);
        }
        else if (sendHeartBeatNum == 5)
        {
            seqRpc->aliveNoity(false);
            sendHeartBeatNum = 0;
        }
    }
}


SequencerRpc::SequencerRpc(int index) : QObject()
{
    _index = index;
    _subSocket = NULL;
    _reqSocket = NULL;
    _subThread = NULL;
    _aliveFlag = 0;
    _isStop = false;
}

SequencerRpc::~SequencerRpc()
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
}

bool SequencerRpc::init(const QString&  pubIp, int pubPort, const QString&  reqIp, int reqPort)
{
    _subSocket = new ZmqSocket(ZMQ_SUB);  
    if (!_subSocket->connect(pubIp.toStdString().c_str(), pubPort))
    {
        LogMsg(Error, "connet sequencer pub failed. ip:%s port %d", pubIp.toStdString().c_str(), pubPort);
        return false;
    }

    _subSocket->setSockOpt(ZMQ_SUBSCRIBE, "", 0);

    _reqSocket = new ZmqSocket(ZMQ_REQ);
    if (!_reqSocket->connect(reqIp.toStdString().c_str(), reqPort))
    {
        LogMsg(Error, "connet sequencer req failed. ip:%s port %d", reqIp.toStdString().c_str(), reqPort);
        return false;
    }

    _reqSocket->setSockOpt(ZMQ_RCVTIMEO, (void*)&TIME_OUT, sizeof(int));
    _reqSocket->setSockOpt(ZMQ_SNDTIMEO, (void*)&TIME_OUT, sizeof(int));

    return true;
}

bool SequencerRpc::start()
{
    _subThread = new std::thread(sub_recvData, this);
    return true;
}

void SequencerRpc::stop()
{
    _isStop = true;
    _subThread->join();
}

bool SequencerRpc::loadProfile(const QString& csvFilePath)
{
    LoadCsvCmdReq* req = new LoadCsvCmdReq();
    req->setParam(csvFilePath);
    Buffer sendbuf;
    req->encode(sendbuf);

    delete req;
    req = NULL;

    if (_reqSocket->sendData(sendbuf) < 0)
    {
        LogMsg(Error, "loadProfile failed. error is : send data failed");
        return false;
    }

    if (_reqSocket->select(ZMQ_POLLIN, TIME_OUT) == 0)
    {
        LogMsg(Error, "loadProfile failed. error is : recv data over time.");
        return false;
    }

    Buffer recvbuf;
    if (_reqSocket->recvData(recvbuf) < 0)
    {
        LogMsg(Error, "loadProfile failed. error is : recv data failed.");
        return false;
    }

    LoadCsvCmdRsp* rsp = new LoadCsvCmdRsp();
    if (!rsp->decode(recvbuf))
    {
        LogMsg(Error, "loadProfile failed. error is : LoadCsvCmdRsp decode failed.");
        delete rsp;
        rsp = NULL;
        return false;
    }

    delete rsp;
    rsp = NULL;
    return true;
}

bool SequencerRpc::getContent(QVector<QString>& items)
{
    ListCmdReq* req = new ListCmdReq();
    Buffer sendbuf;
    req->encode(sendbuf);

    if (_reqSocket->sendData(sendbuf) < 0)
    {
        LogMsg(Error, "getContent failed. error is : send data failed");
        return false;
    }

    if (_reqSocket->select(ZMQ_POLLIN, TIME_OUT) == 0)
    {
        LogMsg(Error, "getContent failed. error is : recv data over time.");
        return false;
    }

    Buffer recvbuf;
    if (_reqSocket->recvData(recvbuf) < 0)
    {
        LogMsg(Error, "getContent failed. error is : recv data failed.");
        return false;
    }

    ListCmdRsp* rsp = new ListCmdRsp();
    if (!rsp->decode(recvbuf))
    {
        LogMsg(Error, "getContent failed. error is : LoadCsvCmdRsp decode failed.");
        delete rsp;
        rsp = NULL;
        return false;
    }

    items = rsp->getItems();

    delete rsp;
    rsp = NULL;

    return true;
}

void SequencerRpc::setAlive(bool flag)
{
    _aliveFlag = (flag ? 1 : 2);
}

bool SequencerRpc::isAlive()
{
    return _aliveFlag == 1 ? true : false;
}


void SequencerRpc::aliveNoity(bool isShow)
{
    if (_aliveFlag != 0)
        emit isAliveSignal(_index, isAlive(), isShow);
}
