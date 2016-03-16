#ifndef COMMAND_H
#define COMMAND_H

#include "buffer.h"
#include <QString>
#include <QVector>

class TCsvDataItem;

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

    void setParam(const QString& param);

private:
    QString _param;
};

class LoadCsvCmdRsp : public CommandBase
{
public:
    LoadCsvCmdRsp();
    ~LoadCsvCmdRsp();

    bool encode(Buffer& buf);
    bool decode(const Buffer& buf);

private:

};

class ListCmdReq : public CommandBase
{
public:
    ListCmdReq();
    ~ListCmdReq();

    bool encode(Buffer& buf);
    bool decode(const Buffer& buf);

    void setParam(const QString& param);

private:
    QString _param;
};

class ListCmdRsp : public CommandBase
{
public:
    ListCmdRsp();
    ~ListCmdRsp();

    bool encode(Buffer& buf);
    bool decode(const Buffer& buf);

private:
    QVector<TCsvDataItem*> _items;
};

#endif // COMMAND_H
