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

ListCsvFileMsg::ListCsvFileMsg()
{
    _id = LIST_CSV_MSG;
    _context = NULL;
    _len = 0;
}

ListCsvFileMsg::~ListCsvFileMsg()
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

ProcItemStateMsg::ProcItemStateMsg()
{
    _id = PROC_ITEMSTATE_MSG;
    _context = NULL;
    _len = 0;

    _itemStart = true;
    _index = -1;
}

ProcItemStateMsg::~ProcItemStateMsg()
{

}

void ProcItemStateMsg::setData(bool isStart, int index, const QString& data)
{
    _itemStart = isStart;
    _index = index;
    _data = data;
}
