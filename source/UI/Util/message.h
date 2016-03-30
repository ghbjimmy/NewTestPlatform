#ifndef MESSAGE_H
#define MESSAGE_H

#include <QVector>
#include <QMap>
#include "structdefine.h"

const int LIST_CSV_MSG = 1000;
const int PROC_ITEMSTATE_MSG = 1001;
const int CHANEL_STATE_MSG = 1002;
const int START_TEST_MSG = 1003;
const int STOP_TEST_MSG = 1004;
const int USERLOGIN_MSG = 1005;

class IMessage
{
public:
    IMessage() {
        _id = -1;
    }

    virtual ~IMessage(){}

    virtual int  messageID() const
    {
        return _id;
    }

protected:
    int _id;
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
