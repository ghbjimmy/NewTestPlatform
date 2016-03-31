#include "message.h"

ListCsvFileMsg::ListCsvFileMsg()
{
    _id = LIST_CSV_MSG;
    _strategyId = Active;
}

ListCsvFileMsg::~ListCsvFileMsg()
{

}

ProcItemStateMsg::ProcItemStateMsg()
{
    _id = PROC_PUBEVENT_MSG;
    _strategyId = Active;
    _evtId = -1;
    _index = -1;
}

ProcItemStateMsg::~ProcItemStateMsg()
{

}

void ProcItemStateMsg::setData(int index, int evt, const QString& data)
{
    _evtId = evt;
    _index = index;
    _data = data;
}


ChannelStateMsg::ChannelStateMsg()
{
    _id = SLOT_STATE_MSG;
    _strategyId = Active;
}

ChannelStateMsg::~ChannelStateMsg()
{

}

void ChannelStateMsg::setData(int index, int result)
{
    _index = index;
    _result = result;
}


StartTestMsg::StartTestMsg()
{
    _id = START_TEST_MSG;
    _strategyId = Active;
}

StartTestMsg::~StartTestMsg()
{

}

StopTestMsg::StopTestMsg()
{
    _id = STOP_TEST_MSG;
    _strategyId = Active;
}

StopTestMsg::~StopTestMsg()
{

}

UserLoginMsg::UserLoginMsg()
{
    _id = USERLOGIN_MSG;
    _strategyId = Active;
}

UserLoginMsg::~UserLoginMsg()
{

}

void UserLoginMsg::setUserPrivils(const QMap<QString, int>& privils)
{
    _widgetPrivils = privils;
}
