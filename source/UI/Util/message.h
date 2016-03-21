#ifndef MESSAGE_H
#define MESSAGE_H

#include <QVector>
#include "structdefine.h"

const int LIST_CSV_MSG = 1000;
const int PROC_ITEMSTATE_MSG = 1001;
const int CHANEL_STATE_MSG = 1002;
const int START_TEST_MSG = 1003;

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

class LoadScopeViewMsg : public IMessage
{
public:
    LoadScopeViewMsg();
    ~LoadScopeViewMsg();
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

//处理进度消息
class ProcItemStateMsg : public IMessage
{
public:
    ProcItemStateMsg();
    ~ProcItemStateMsg();

    void setData(bool isStart, int index, const QString& data);
    inline bool isItemStart() const {return _itemStart;}
    inline int getIndex() const {return _index;}
    inline const QString& getData() const {return _data;}

private:
    bool _itemStart;
    int _index;
    QString _data;
};

//指定定通道的状态变化消息
class ChannelStateMsg : public IMessage
{
public:
    ChannelStateMsg();
    ~ChannelStateMsg();

    void setData(int index, int result = -1);
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

#endif
