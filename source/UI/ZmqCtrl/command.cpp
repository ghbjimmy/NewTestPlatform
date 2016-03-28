#include "command.h"
#include "qlog.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QVariant>
#include <QUuid>

static void setupBuffer(Buffer& buf, const QString& funName, const QString& param)
{
    QVariantMap cmdMap;
    cmdMap.insert("function", funName);

    QVariantList params;
    params.append(param);
    cmdMap.insert("params", params);
    cmdMap.insert("jsonrpc", "1.0");
    cmdMap.insert("id", QUuid::createUuid().toString());

    QJsonDocument document = QJsonDocument::fromVariant(cmdMap);
    QByteArray array = document.toJson();

    QString str(array);
    str = str.remove('\n');
    str = str.remove(' ');
    str = str.trimmed();
    buf.setBuf(str.toLocal8Bit().data(), str.length());
}

CommandBase::CommandBase()
{

}

CommandBase::~CommandBase()
{

}

LoadCsvCmdReq::LoadCsvCmdReq()
{

}


LoadCsvCmdReq::~LoadCsvCmdReq()
{

}

bool LoadCsvCmdReq::encode(Buffer& buf)
{
    setupBuffer(buf, "load", _param);
    return true;
}

bool LoadCsvCmdReq::decode(const Buffer& buf)
{
    //QJsonDocument document = QJsonDocument::fromRawData(buf.getBuf(), buf.getLen());
    return true;
}

void LoadCsvCmdReq::setParam(const QString& param)
{
    _param = param;
}

LoadCsvCmdRsp::LoadCsvCmdRsp()
{
    _isSuccess = false;
}

LoadCsvCmdRsp::~LoadCsvCmdRsp()
{

}

bool LoadCsvCmdRsp::encode(Buffer& buf)
{
    return true;
}

bool LoadCsvCmdRsp::decode(const Buffer& buf)
{
    if (!buf.isValid())
        return false;

    QByteArray array(buf.getBuf(), buf.getLen());
    QJsonParseError json_error;
    QJsonDocument document = QJsonDocument::fromJson(array, &json_error);

    if(json_error.error != QJsonParseError::NoError)
    {
        LogMsg(Error, "Parse json failed.");
        return false;
    }

    QJsonObject obj = document.object();
    if(obj.contains("result"))
    {
        _result = obj.value("result").toString();
        _isSuccess = true;
    }
    else if (obj.contains("error"))
    {
        _result = obj.value("error").toString();
        _isSuccess = false;
    }

    return true;
}

ListCmdReq::ListCmdReq()
{
    _param = "all";
}

ListCmdReq::~ListCmdReq()
{

}

bool ListCmdReq::encode(Buffer& buf)
{
    setupBuffer(buf, "list", _param);
    return true;
}

bool ListCmdReq::decode(const Buffer& buf)
{
    return true;
}

void ListCmdReq::setParam(const QString& param)
{
    _param = param;
}

ListCmdRsp::ListCmdRsp()
{

}

ListCmdRsp::~ListCmdRsp()
{
    _items.clear();
}

bool ListCmdRsp::encode(Buffer& buf)
{
    return true;
}

bool ListCmdRsp::decode(const Buffer& buf)
{
    if (!buf.isValid())
        return false;

    QByteArray byteArray(buf.getBuf(), buf.getLen());

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
    QJsonValue value;
    if(obj.contains("result"))
    {
        value = obj.value("result");
        if (!value.isArray())
        {
            LogMsg(Error, "json format is error:'result' is not array.");
            return false;
        }

        QJsonArray resultArr = value.toArray();
        int size = resultArr.size();
        for (int i = 0; i < size; ++i)
        {
            if (!resultArr.at(i).isArray())
            {
                LogMsg(Error, "json format is error: data is not array.");
                return false;
            }

            if (i == 0)
            {//parse num
                QJsonArray numArr = resultArr.at(i).toArray();
                int cnt = numArr.size();
            }
            else
            {
                QJsonArray dataArr = resultArr.at(i).toArray();
                if (dataArr.size() != 2)
                {
                    LogMsg(Error, "json format is error: data array count is not eq 2.");
                    return false;
                }

                int index = dataArr.at(0).toInt();
                QString str = dataArr.at(1).toString();
                _items.push_back(str);
            }
        }
    }

    return true;
}

SimpleFuncCmdReq::SimpleFuncCmdReq()
{

}

SimpleFuncCmdReq::~SimpleFuncCmdReq()
{

}

bool SimpleFuncCmdReq::encode(Buffer& buf)
{
    QVariantMap cmdMap;
    cmdMap.insert("function", _funcName);

    QJsonDocument document = QJsonDocument::fromVariant(cmdMap);
    QByteArray array = document.toJson();

    QString str(array);
    str = str.remove('\n');
    str = str.remove(' ');
    buf.setBuf(str.toLocal8Bit().data(), str.length());
    return true;
}

bool SimpleFuncCmdReq::decode(const Buffer& buf)
{
    return true;
}

SimpleFuncCmdRsp::SimpleFuncCmdRsp()
{

}

SimpleFuncCmdRsp::~SimpleFuncCmdRsp()
{

}

bool SimpleFuncCmdRsp::encode(Buffer& buf)
{
    return true;
}

bool SimpleFuncCmdRsp::decode(const Buffer& buf)
{
    if (!buf.isValid())
        return false;

    QByteArray byteArray(buf.getBuf(), buf.getLen());

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
    QString result = "";
    if(obj.contains("status"))
    {
        result = obj.value("status").toString();
    }

    if (result != "ok")
        return false;
    return true;
}
