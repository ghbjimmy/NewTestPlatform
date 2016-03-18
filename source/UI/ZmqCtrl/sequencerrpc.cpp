#include "sequencerrpc.h"
#include "zmqsocket.h"
#include "qlog.h"
#include "command.h"
#include "const.h"
#include "structdefine.h"

#include <thread>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

//获取消息的类型 0:心跳； 1: item_start; 2: item_end; -1:unknown
static int getMsgType(const QString& msg)
{
    if (msg.contains("FCT_HEARTBEAT"))
    {
        return 0;
    }
    else if (msg.contains("group") && msg.contains("tid"))
    {
        return 1;
    }
    else if (msg.contains("tid") && msg.contains("value"))
    {
        return 2;
    }

    return -1;
}

SequencerRpc::SequencerRpc(int index) : BaseRpc(index)
{

}

SequencerRpc::~SequencerRpc()
{

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

bool SequencerRpc::procSubRecvMsg(const QString& msg)
{
    return true;
}

bool SequencerRpc::procItemStart(const QString& msg)
{
    QJsonParseError json_error;
    QJsonDocument document = QJsonDocument::fromJson(msg.toUtf8(), &json_error);
    if(json_error.error != QJsonParseError::NoError)
    {
        LogMsg(Error, "Parse ItemStart json failed. %s", msg.toStdString().c_str());
        return false;
    }

    if (!document.isObject())
    {
        LogMsg(Error, "Parse ItemStar json format is error. %s", msg.toStdString().c_str());
        return false;
    }

    TItemStart itemStart;
    QJsonObject obj = document.object();
    if(obj.contains("group"))
    {
        itemStart.group = obj.take("group").toString();
    }
    if (obj.contains("tid"))
    {
        itemStart.tid = obj.take("tid").toString();
    }
    if (obj.contains("unit"))
    {
        itemStart.unit = obj.take("unit").toString();
    }
    if (obj.contains("low"))
    {
        itemStart.low = obj.take("low").toString();
    }
    if (obj.contains("high"))
    {
        itemStart.high = obj.take("high").toString();
    }
    if (obj.contains("pdca"))
    {
        itemStart.pdca = obj.take("pdca").toString();
    }

    return true;
}

bool SequencerRpc::procItemEnd(const QString& msg)
{
    return true;
}
