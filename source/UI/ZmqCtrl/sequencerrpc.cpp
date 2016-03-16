#include "sequencerrpc.h"
#include "zmqsocket.h"
#include "qlog.h"
#include "command.h"

#include <thread>

const int& TIME_OUT = 3000;

static void sub_recvFun(void* obj)
{
    SequencerRpc* seqRpc = (SequencerRpc*)obj;

    while(1)
    {
        ZmqSocket* subSocket = seqRpc->getSubSocket();
        if (subSocket->select(ZMQ_POLLIN, -1) <= 0)
        {
            LogMsg(Error, "sub socket select failed.");
            continue;
        }


    }
}

SequencerRpc::SequencerRpc(int index)
{
    _index = index;
    _subSocket = NULL;
    _reqSocket = NULL;
    _subThread = NULL;
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

bool SequencerRpc::init(const char *subIp, int subPort, const char *reqIp, int reqPort)
{
    _subSocket = new ZmqSocket(ZMQ_SUB);
    if (!_subSocket->connect(subIp, subPort))
    {
        LogMsg(Error, "connet sequencer pub failed. ip:%s port %d", subIp, subPort);
        return false;
    }

    _reqSocket = new ZmqSocket(ZMQ_REQ);
    _reqSocket->setSendTimeOut(TIME_OUT);
    _reqSocket->setRecvTimeOut(TIME_OUT);
    if (!_reqSocket->connect(reqIp, reqPort))
    {
        LogMsg(Error, "connet sequencer req failed. ip:%s port %d", reqIp, reqPort);
        return false;
    }

    return true;
}

bool SequencerRpc::start()
{


    _subThread = new std::thread(sub_recvFun, this);
    return true;
}

void SequencerRpc::stop()
{

}

bool SequencerRpc::loadProfile(const char* csvFilePath)
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

bool SequencerRpc::getContent(QVector<TCsvDataItem*>& items)
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

    delete rsp;
    rsp = NULL;

    return true;
}
