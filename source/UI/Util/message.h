#ifndef MESSAGE_H
#define MESSAGE_H


#include "plugin_global.h"

const int LIST_CSV_MSG = 1000;
const int PROC_ITEMSTATE_MSG = 1001;

class IMessage
{
public:
    IMessage() {
        _id = -1;
        _context = NULL;
        _len = -1;
    }

    virtual ~IMessage(){}

    virtual int  messageID() const
    {
        return _id;
    }

    virtual void* context() const
    {
        return _context;
    }

    virtual int length() const
    {
        return _len;
    }

protected:
    int _id;
    void* _context;
    int _len;
};

class ChannelStateMsg : public IMessage
{
public:
    ChannelStateMsg();
    ~ChannelStateMsg();
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

class LoadScopeViewMsg : public IMessage
{
public:
    LoadScopeViewMsg();
    ~LoadScopeViewMsg();
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

#endif
