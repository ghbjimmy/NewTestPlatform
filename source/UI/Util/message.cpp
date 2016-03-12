#include "message.h"

ChannelStateMsg::ChannelStateMsg()
{
    _id = 100;
    _context = NULL;
    _length = 20;
}

ChannelStateMsg::~ChannelStateMsg()
{

}

int ChannelStateMsg::messageID() const
{
    return _id;
}

void* ChannelStateMsg::context() const
{
    return _context;
}

int ChannelStateMsg::length() const
{
    return _length;
}
