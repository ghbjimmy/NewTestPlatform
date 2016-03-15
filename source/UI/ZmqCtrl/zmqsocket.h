#ifndef ZMQSOCKET_H
#define ZMQSOCKET_H

#include <string>
#include "buffer.h"

#define ZMQ_POLLIN 1
#define ZMQ_POLLOUT 2
#define ZMQ_POLLERR 4

class ZmqSocket
{
public:
    ZmqSocket(int type);//ZMQ_PUB=1 ZMQ_SUB ZMQ_REQ ZMQ_REP
    ~ZmqSocket();

    bool connect(const char* szSvrIp, int port);
    int bind(const char* szSvrIp, int port);
    int sendData(const Buffer& buf);
    int recvData(Buffer& buf);

    int setRecvTimeOut(int timeout);
    int setSendTimeOut(int timeout);

    int select(int evt, int timeout = -1);

private:
    void* _context;
    void* _socket;
    std::string _ip;
    int _port;
    int _type;
};

#endif // ZMQSOCKET_H
