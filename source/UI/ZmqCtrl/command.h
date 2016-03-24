#ifndef COMMAND_H
#define COMMAND_H

#include "buffer.h"

#include <QString>
#include <QVector>

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

    inline bool isSuccess() const {return _isSuccess;}
    inline const QString& getResult() const {return _result;}

private:
    QString _result;
    bool _isSuccess;
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

    const QVector<QString>& getItems() const {return _items;}

private:
    QVector<QString> _items;
};

class SimpleFuncCmdReq : public CommandBase
{
public:
    SimpleFuncCmdReq();
    ~SimpleFuncCmdReq();

    bool encode(Buffer& buf);
    bool decode(const Buffer& buf);

    inline void setStartFun(const QString& func) {_funcName = func;}

private:
    QString _funcName;
};

class SimpleFuncCmdRsp : public CommandBase
{
public:
    SimpleFuncCmdRsp();
    ~SimpleFuncCmdRsp();

    bool encode(Buffer& buf);
    bool decode(const Buffer& buf);

    inline bool isSuccess() const {return _isSuccess;}

private:
     bool _isSuccess;
};
#endif // COMMAND_H
