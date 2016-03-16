#include "sequencerrpc.h"
#include "zmqsocket.h"
#include "qlog.h"
#include "command.h"

const int& TIME_OUT = 3000;

SequencerRpc::SequencerRpc()
{
    _pubSocket = NULL;
    _reqSocket = NULL;
}

SequencerRpc::~SequencerRpc()
{
    if (_reqSocket != NULL)
    {
        delete _reqSocket;
        _reqSocket = NULL;
    }

    if (_pubSocket != NULL)
    {
        delete _pubSocket;
        _pubSocket = NULL;
    }
}

bool SequencerRpc::init()
{
    return true;
}

bool SequencerRpc::start(const char *pubIp, int pubPort, const char *reqIp, int reqPort)
{
    _pubSocket = new ZmqSocket(ZMQ_SUB);
    if (!_pubSocket->connect(pubIp, pubPort))
    {
        LogMsg(Error, "connet sequencer pub failed. ip:%s port %d", pubIp, pubPort);
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

void SequencerRpc::stop()
{

}

bool SequencerRpc::loadProfile(const char* csvFilePath, std::string& contentJson)
{
    LoadCsvCmdReq* req = new LoadCsvCmdReq();
    req->setParam(csvFilePath);
    Buffer sendbuf;
    if (!req->encode(sendbuf))
    {
        LogMsg(Error, "loadProfile failed. error is : LoadCsvCmdReq encode failed. path:%s", csvFilePath);
        delete req;
        req = NULL;
        return false;
    }

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
        return false;
    }

    return true;
}
