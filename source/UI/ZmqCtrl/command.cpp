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

}

bool ListCmdRsp::encode(Buffer& buf)
{
    return true;
}

static void parseCsvItem(QJsonObject& obj,TCsvDataItem* item)
{
    if (obj.contains("FUNCTION"))
    {
        item->function = obj.take("FUNCTION").toString();
    }

    if (obj.contains("PARAM1"))
    {
        item->param1 = obj.take("PARAM1").toString();
    }

    if (obj.contains("GROUP"))
    {
        item->group = obj.take("GROUP").toString();
    }

    if (obj.contains("DESCRIPTION"))
    {
        item->desc = obj.take("DESCRIPTION").toString();
    }

    if (obj.contains("VAL"))
    {
        item->val = obj.take("VAL").toString();
    }

    if (obj.contains("HIGH"))
    {
        item->high = obj.take("HIGH").toString();
    }

    if (obj.contains("TIMEOUT"))
    {
        item->timeout = obj.take("TIMEOUT").toString();
    }

    if (obj.contains("PARAM2"))
    {
        item->param2 = obj.take("PARAM2").toString();
    }

    if (obj.contains("KEY"))
    {
        item->key = obj.take("KEY").toString();
    }

    if (obj.contains("TID"))
    {
        item->tid = obj.take("TID").toString();
    }

    if (obj.contains("UNIT"))
    {
        item->unit = obj.take("UNIT").toString();
    }

    if (obj.contains("LOW"))
    {
        item->low = obj.take("LOW").toString();
    }
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
        value = obj.take("result");
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
                QJsonObject itemObj = dataArr.at(1).toObject();

                TCsvDataItem* item = new TCsvDataItem();
                parseCsvItem(itemObj, item);

                _items.push_back(item);
            }
        }
    }

    return true;
}

GetItemStartSub::GetItemStartSub()
{

}

GetItemStartSub::~GetItemStartSub()
{

}

bool GetItemStartSub::encode(Buffer& buf)
{
    return true;
}

bool GetItemStartSub::decode(const Buffer& buf)
{
    return true;
}

GetItemEndSub::GetItemEndSub()
{

}

GetItemEndSub::~GetItemEndSub()
{

}

bool GetItemEndSub::encode(Buffer& buf)
{
    return true;
}

bool GetItemEndSub::decode(const Buffer& buf)
{
    return true;
}
