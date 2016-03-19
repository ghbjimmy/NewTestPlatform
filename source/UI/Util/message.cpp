#include "message.h"

ListCsvFileMsg::ListCsvFileMsg()
{
    _id = LIST_CSV_MSG;
}

ListCsvFileMsg::~ListCsvFileMsg()
{

}

LoadScopeViewMsg::LoadScopeViewMsg()
{
    _id = 130;
}

LoadScopeViewMsg::~LoadScopeViewMsg()
{

}

ProcItemStateMsg::ProcItemStateMsg()
{
    _id = PROC_ITEMSTATE_MSG;

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

ChannelStateMsg::ChannelStateMsg()
{
    _id = CHANEL_STATE_MSG;
}

ChannelStateMsg::~ChannelStateMsg()
{

}

void ChannelStateMsg::setData(int index, int result)
{
    _index = index;
    _result = result;
}
