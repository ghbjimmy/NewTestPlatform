#ifndef ZMQSOCKET_H
#define ZMQSOCKET_H

#include <string>
#include "buffer.h"

#include "zmq.h"

class ZmqSocket
{
public:
    ZmqSocket(int type);//ZMQ_PUB=1 ZMQ_SUB ZMQ_REQ ZMQ_REP
    ~ZmqSocket();

    bool connect(const char* szSvrIp, int port);
    int bind(const char* szSvrIp, int port);
    int sendData(const Buffer& buf);

    int sendData(const char* szData, int len);

    int recvData(Buffer& buf);

    int setSockOpt(int sockOpt, void* val, int len); //ZMQ_RCVTIMEO ZMQ_SNDTIMEO ZMQ_SUBSCRIBE

    int select(int evt, int timeout = -1);

    inline int getSockType() const {return _type;}
    inline const std::string& getIp() const {return _ip;}
    inline int getPort() const {return _port;}

private:
    void* _context;
    void* _socket;
    std::string _ip;
    int _port;
    int _type;
};

#endif // ZMQSOCKET_H
