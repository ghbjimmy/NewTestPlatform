#ifndef COMMAND_H
#define COMMAND_H

#include "buffer.h"

class CommandBase
{
public:
    CommandBase();
    virtual ~CommandBase();

    virtual bool encode(Buffer& buf) = 0;
    virtual bool decode(const Buffer& buf) = 0;

private:

};

class LoadCsvCmdReq : public CommandBase
{
public:
    LoadCsvCmdReq();
    ~LoadCsvCmdReq();

    bool encode(Buffer& buf);
    bool decode(const Buffer& buf);

private:

};

class LoadCsvCmdRsp : public CommandBase
{
public:
    LoadCsvCmdRsp();
    ~LoadCsvCmdRsp();

    bool encode(Buffer& buf);
    bool decode(const Buffer& buf);
};

class ListCmdReq : public CommandBase
{
public:
    ListCmdReq();
    ~ListCmdReq();

    bool encode(Buffer& buf);
    bool decode(const Buffer& buf);
};

class ListCmdRsp : public CommandBase
{
public:
    ListCmdRsp();
    ~ListCmdRsp();

    bool encode(Buffer& buf);
    bool decode(const Buffer& buf);
};

#endif // COMMAND_H
