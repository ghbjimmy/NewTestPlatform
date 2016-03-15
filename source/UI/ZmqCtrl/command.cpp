#include "command.h"

CommandBase::CommandBase()
{

}

CommandBase::~CommandBase()
{

}

LoadCsvCmdReq::LoadCsvCmdReq()
{

}


LoadCsvCmdReq::~LoadCsvCmdReq()
{

}

bool LoadCsvCmdReq::encode(Buffer& buf)
{
    return true;
}

bool LoadCsvCmdReq::decode(const Buffer& buf)
{
    return true;
}

LoadCsvCmdRsp::LoadCsvCmdRsp()
{

}

LoadCsvCmdRsp::~LoadCsvCmdRsp()
{

}

bool LoadCsvCmdRsp::encode(Buffer& buf)
{
    return true;
}

bool LoadCsvCmdRsp::decode(const Buffer& buf)
{
    return true;
}

ListCmdReq::ListCmdReq()
{

}

ListCmdReq::~ListCmdReq()
{

}

bool ListCmdReq::encode(Buffer& buf)
{
    return true;
}

bool ListCmdReq::decode(const Buffer& buf)
{
    return true;
}
