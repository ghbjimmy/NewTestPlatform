#include "zmqcfgparser.h"
#include "qlog.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>


const QString& SEQUENCER_PORT = "SEQUENCER_PORT";
const QString& SEQUENCER_PUB = "SEQUENCER_PUB";
const QString& SM_PUB = "SM_PUB";
const QString& SM_PORT = "SM_PORT";

ZmqCfgParser::ZmqCfgParser()
{
    _seqPort = -1;
    _seqPub = -1;
    _smPort = -1;
    _smPub = -1;
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
    QJsonDocument doucment = QJsonDocument::fromJson(byteArray, &json_error);
    if(json_error.error != QJsonParseError::NoError)
    {
        LogMsg(Error, "Parse json failed.");
        return false;
    }

    if (!doucment.isObject())
    {
        LogMsg(Error, "json format is error.");
        return false;
    }

    QJsonObject obj = doucment.object();
    if(obj.contains(SEQUENCER_PORT))
    {
        QJsonValue value = obj.take(SEQUENCER_PORT);
        _seqPort = value.toInt();
    }

    if(obj.contains(SEQUENCER_PUB))
    {
        QJsonValue value = obj.take(SEQUENCER_PUB);
        _seqPub = value.toInt();
    }

    if(obj.contains(SM_PUB))
    {
        QJsonValue value = obj.take(SM_PUB);
        _smPub = value.toInt();
    }

    if(obj.contains(SM_PORT))
    {
        QJsonValue value = obj.take(SM_PORT);
        _smPort = value.toInt();
    }

    return true;
}
