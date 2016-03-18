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
            int msgType = getMsgType(recvMsg);
            switch (msgType)
            {
                case 0:
                {
                    seqRpc->setAlive(true);
                    timeoutNum = 0;
                    break;
                }
                case 1:
                {
                    seqRpc->procItemStart(recvMsg);
                    break;
                }
                case 2:
                {
                    seqRpc->procItemEnd(recvMsg);
                    break;
                }
                default:
                {
                    LogMsg(Error, "recv unknwon msg type : %s", recvMsg.toStdString().c_str());
                    break;
                }
            }
        }

        //控制心跳指示灯闪
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

    if (_subThread != NULL)
    {
        delete _subThread;
        _subThread = NULL;
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
