#ifndef MESSAGE_H
#define MESSAGE_H


#include "plugin_global.h"

class ChannelStateMsg : public IMessage
{
public:
    ChannelStateMsg();
    ~ChannelStateMsg();

    virtual int  messageID() const;
    virtual void* context() const;
    virtual int   length() const;

private:
    int _id;
    int _length;
    void* _context;
};

#endif
