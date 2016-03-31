#ifndef MESSAGE_H
#define MESSAGE_H

#include <QVector>
#include <QMap>
#include "structdefine.h"
#include "const.h"

const int LIST_CSV_MSG = 0x100001; //load csv 消息
const int PROC_PUBEVENT_MSG = 0x100002; //处理 pub event 消息 包括seq start/end,  item start/end
const int SLOT_STATE_MSG = 0x100003;  //通道状态消息
const int START_TEST_MSG = 0x100004;  //开始测试消息
const int STOP_TEST_MSG = 0x100005;   //停止测试消息
const int USERLOGIN_MSG = 0x100006;   //用户登录消息

class IMessage
{
public:
    IMessage() {
        _id = -1;
        _strategyId = Active;
    }

    virtual ~IMessage(){}

    int groupType() const //分组id 跟插件的pluginType相对应
    {
        return _id >> 16;
    }

    int messageID() const //消息id
    {
         return _id;
    }

    int strategyID() const //策略id
    {
        return _strategyId;
    }

protected:
    int _id;
    int _strategyId;
};


//处理csv内容消息
class ListCsvFileMsg : public IMessage
{
public:
    ListCsvFileMsg();
    ~ListCsvFileMsg();

    void setData(const QVector<QString>& items) {_dataItems = items;}
    inline const QVector<QString>& getItems() const {return _dataItems;}

private:
    QVector<QString> _dataItems;
};

//处理seq事件消息 itemStart itemEnd seqStart seqEnd
class ProcItemStateMsg : public IMessage
{
public:
    ProcItemStateMsg();
    ~ProcItemStateMsg();

    void setData(int index, int evt, const QString& data);
    inline int getEventId() const {return _evtId;}
    inline int getIndex() const {return _index;}
    inline const QString& getData() const {return _data;}

private:
    int _evtId;
    int _index;
    QString _data;
};

//指定定通道的状态变化消息
class ChannelStateMsg : public IMessage
{
public:
    ChannelStateMsg();
    ~ChannelStateMsg();

    void setData(int index, int result);
    inline int getIndex() const {return _index;}
    inline int getResult() const {return _result;}

private:
    int _index;
    int _result;
};


//开始测试命令消息
class StartTestMsg : public IMessage
{
public:
    StartTestMsg();
    ~StartTestMsg();

private:

};

//停止测试消息
class StopTestMsg : public IMessage
{
public:
    StopTestMsg();
    ~StopTestMsg();

private:

};

//用户登录消息
class UserLoginMsg : public IMessage
{
public:
    UserLoginMsg();
    ~UserLoginMsg();
    void setUserPrivils(const QMap<QString, int>& privils);
    inline const QMap<QString, int>& getUserPrivils() const {return _widgetPrivils;}

private:
    QMap<QString, int> _widgetPrivils;
};

#endif
