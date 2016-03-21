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

bool StateMachineRpc::StartTest()
{
    StartTestCmdReq* req = new StartTestCmdReq();
    req->setStartFun("START");
    Buffer sendbuf;
    req->encode(sendbuf);

    if (_reqSocket->sendData(sendbuf) < 0)
    {
        LogMsg(Error, "StartTest failed. error is : send data failed");
        return false;
    }

    if (_reqSocket->select(ZMQ_POLLIN, TIME_OUT) == 0)
    {
        LogMsg(Error, "StartTest failed. error is : recv data over time.");
        return false;
    }

    Buffer recvbuf;
    if (_reqSocket->recvData(recvbuf) < 0)
    {
        LogMsg(Error, "StartTest failed. error is : recv data failed.");
        return false;
    }

    StartTestCmdRsp* rsp = new StartTestCmdRsp();
    if (!rsp->decode(recvbuf))
    {
        LogMsg(Error, "StartTest failed. error is : LoadCsvCmdRsp decode failed.");
        delete rsp;
        rsp = NULL;
        return false;
    }

    bool ret = rsp->isSuccess();
    delete rsp;
    rsp = NULL;

    return ret;
}
