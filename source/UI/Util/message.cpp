#include "message.h"

ListCsvFileMsg::ListCsvFileMsg()
{
    _id = LIST_CSV_MSG;
}

ListCsvFileMsg::~ListCsvFileMsg()
{

}

ProcItemStateMsg::ProcItemStateMsg()
{
    _id = PROC_ITEMSTATE_MSG;

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


StartTestMsg::StartTestMsg()
{
    _id = START_TEST_MSG;
}

StartTestMsg::~StartTestMsg()
{

}

StopTestMsg::StopTestMsg()
{
    _id = STOP_TEST_MSG;
}

StopTestMsg::~StopTestMsg()
{

}

UserLoginMsg::UserLoginMsg()
{
    _id = USERLOGIN_MSG;
}

UserLoginMsg::~UserLoginMsg()
{

}

void UserLoginMsg::setUserPrivils(const QMap<QString, int>& privils)
{
    _widgetPrivils = privils;
}
