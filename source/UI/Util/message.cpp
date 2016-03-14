#include "message.h"

ChannelStateMsg::ChannelStateMsg()
{
    _id = 100;
    _context = NULL;
    _len = 20;
}

ChannelStateMsg::~ChannelStateMsg()
{

}

LoadCsvFileMsg::LoadCsvFileMsg()
{
    _id = 120;
    _context = NULL;
    _len = 120;
}

LoadCsvFileMsg::~LoadCsvFileMsg()
{

}

LoadScopeViewMsg::LoadScopeViewMsg()
{
    _id = 130;
    _context = NULL;
    _len = 130;
}

LoadScopeViewMsg::~LoadScopeViewMsg()
{

}
