#include "zmqcfgparser.h"
#include "qlog.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

const QString& SEQUENCER_PORT = "SEQUENCER_PORT";
const QString& SEQUENCER_PUB = "SEQUENCER_PUB";
const QString& SM_PUB = "SM_PUB";
const QString& SM_PORT = "SM_PORT";
const QString& TEST_ENGINE_PORT = "TEST_ENGINE_PORT";
const QString& TEST_ENGINE_PUB = "TEST_ENGINE_PUB";

ZmqCfgParser::ZmqCfgParser()
{
    _seqPort = -1;
    _seqPub = -1;
    _smPort = -1;
    _smPub = -1;
    _testEnginePort = -1;
    _testEnginePub = -1;
}

ZmqCfgParser::~ZmqCfgParser()
{

}

bool ZmqCfgParser::parse(const QString& path)
{
    QFile* file = new QFile(path);
    if (!file->exists())
    {
        LogMsg(Error, "config file is not exist.[%s]", path.toStdString().c_str());

        return false;
    }

    if (!file->open(QIODevice::ReadOnly))
    {
        LogMsg(Error, "open file failed.[%s]", path.toStdString().c_str());
        return false;
    }

    QByteArray byteArray = file->readAll();
    file->close();

    QJsonParseError json_error;
    QJsonDocument document = QJsonDocument::fromJson(byteArray, &json_error);
    if(json_error.error != QJsonParseError::NoError)
    {
        LogMsg(Error, "Parse json failed.");
        return false;
    }

    if (!document.isObject())
    {
        LogMsg(Error, "json format is error.");
        return false;
    }

    QJsonObject obj = document.object();
    QJsonValue value;
    if(obj.contains(SEQUENCER_PORT))
    {
        value = obj.take(SEQUENCER_PORT);
        _seqPort = value.toInt();
    }

    if(obj.contains(SEQUENCER_PUB))
    {
        value = obj.take(SEQUENCER_PUB);
        _seqPub = value.toInt();
    }

    if(obj.contains(SM_PUB))
    {
        value = obj.take(SM_PUB);
        _smPub = value.toInt();
    }

    if(obj.contains(SM_PORT))
    {
        value = obj.take(SM_PORT);
        _smPort = value.toInt();
    }

    if(obj.contains(TEST_ENGINE_PORT))
    {
        value = obj.take(TEST_ENGINE_PORT);
        _testEnginePort = value.toInt();
    }

    if(obj.contains(TEST_ENGINE_PUB))
    {
        value = obj.take(TEST_ENGINE_PUB);
        _testEnginePub = value.toInt();
    }

    static const char* ip = "172.15.3.78";
    //_seqIp = ip;
    _seqIp = "127.0.0.1";
    _engineIp = "127.0.0.1";
    //_engineIp = ip;
    _smIp = ip;

    return true;
}
