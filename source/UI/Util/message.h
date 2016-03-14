#ifndef MESSAGE_H
#define MESSAGE_H


#include "plugin_global.h"

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

class LoadCsvFileMsg : public IMessage
{
public:
    LoadCsvFileMsg();
    ~LoadCsvFileMsg();
};

class LoadScopeViewMsg : public IMessage
{
public:
    LoadScopeViewMsg();
    ~LoadScopeViewMsg();
};

#endif
