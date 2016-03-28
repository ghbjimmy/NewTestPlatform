#include "sequencerrpc.h"
#include "zmqsocket.h"
#include "qlog.h"
#include "command.h"
#include "const.h"

#include <thread>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

const int ITEM_START = 2;
const int ITEM_FINISH = 3;
const int SEQ_START = 0;
const int SEQ_END = 1;

//获取消息的类型 0:seqStart 1: seqEnd  2: item_start; 3: item_end; -1:unknown
static int getMsgType(const QString& msg, QString& data)
{
    int ret = -1;
    QJsonParseError json_error;
    QJsonDocument document = QJsonDocument::fromJson(msg.toUtf8(), &json_error);
    if(json_error.error != QJsonParseError::NoError)
    {
        LogMsg(Error, "Parse msg json failed. %s", msg.toStdString().c_str());
        return ret;
    }

    if (!document.isObject())
    {
        LogMsg(Error, "Parse msg json format is error. %s", msg.toStdString().c_str());
        return ret;
    }

    QJsonObject obj = document.object();
    if (obj.contains("event"))
    {
        ret = obj.value("event").toInt();

        if (!obj.contains("data"))
        {
            return -1;
        }

        QJsonObject jsonObject = obj.value("data").toObject();;
        data = QJsonDocument(jsonObject).toJson();
    }

    return ret;
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
        setAlive(false);
        return false;
    }

    if (_reqSocket->select(ZMQ_POLLIN, TIME_OUT) == 0)
    {
        LogMsg(Error, "loadProfile failed. error is : recv data over time.");
        setAlive(false);
        return false;
    }

    Buffer recvbuf;
    if (_reqSocket->recvData(recvbuf) < 0)
    {
        LogMsg(Error, "loadProfile failed. error is : recv data failed.");
        setAlive(false);
        return false;
    }

    LoadCsvCmdRsp* rsp = new LoadCsvCmdRsp();
    if (!rsp->decode(recvbuf))
    {
        LogMsg(Error, "loadProfile failed. error is : LoadCsvCmdRsp decode failed.");
        setAlive(false);
        delete rsp;
        rsp = NULL;
        return false;
    }

    bool ret = rsp->isSuccess();
    delete rsp;
    rsp = NULL;

    return ret;
}

bool SequencerRpc::getCsvContent(QVector<QString>& items)
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
    if (msg.isEmpty())
        return false;

    QString data;
    int msgType = getMsgType(msg, data);
    switch(msgType)
    {
    case ITEM_START:
    case ITEM_FINISH:
    case SEQ_START:
    case SEQ_END:
    {
        emit eventSignal(_index, msgType, data);
        break;
    }
    default:
    {
        return false;
    }
    }

    return true;
}

