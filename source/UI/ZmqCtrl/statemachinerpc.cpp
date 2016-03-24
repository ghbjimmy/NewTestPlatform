#include "statemachinerpc.h"
#include "command.h"
#include "buffer.h"
#include "zmqsocket.h"
#include "qlog.h"
#include "const.h"

StateMachineRpc::StateMachineRpc(int index) : BaseRpc(index)
{

}

StateMachineRpc::~StateMachineRpc()
{

}

bool StateMachineRpc::procSubRecvMsg(const QString& msg)
{
    return true;
}

bool StateMachineRpc::startTest()
{
    const QString& func = "START";
    return execFunc(func);
}

bool StateMachineRpc::stopTest()
{
    const QString& func = "ABORT";
    return execFunc(func);
}

bool StateMachineRpc::execFunc(const QString& func)
{
    SimpleFuncCmdReq* req = new SimpleFuncCmdReq();
    req->setStartFun(func);
    Buffer sendbuf;
    req->encode(sendbuf);

    if (_reqSocket->sendData(sendbuf) < 0)
    {
        LogMsg(Error, "execFunc: %s failed. error is : send data failed", func.toStdString().c_str());
        return false;
    }

    if (_reqSocket->select(ZMQ_POLLIN, TIME_OUT) == 0)
    {
        LogMsg(Error, "execFunc: %s failed. error is : recv data over time.", func.toStdString().c_str());
        return false;
    }

    Buffer recvbuf;
    if (_reqSocket->recvData(recvbuf) < 0)
    {
        LogMsg(Error, "execFunc: %s failed failed. error is : recv data failed.", func.toStdString().c_str());
        return false;
    }

    SimpleFuncCmdRsp* rsp = new SimpleFuncCmdRsp();
    if (!rsp->decode(recvbuf))
    {
        LogMsg(Error, "execFunc: %s failed. error is : SimpleFuncCmdRsp decode failed.", func.toStdString().c_str());
        delete rsp;
        rsp = NULL;
        return false;
    }

    bool ret = rsp->isSuccess();
    delete rsp;
    rsp = NULL;

    return ret;
}
