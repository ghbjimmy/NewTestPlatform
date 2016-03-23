#include <vector>

#include "zmqsocket.h"
#include "qlog.h"

ZmqSocket::ZmqSocket(int type)
{
    _context = NULL;
    _socket = NULL;
    _port = -1;
    _type = type;
}

ZmqSocket::~ZmqSocket()
{
    if (_socket != NULL)
    {
        zmq_close(_socket);
        _socket = NULL;
    }

    if (_context != NULL)
    {
       // zmq_ctx_destroy(_context);
       // _context = NULL;
    }
}

std::string createAddress(const char* ip, int port)
{
    char szAdress[64];
    sprintf(szAdress, "tcp://%s:%d", ip, port);
    return std::string(szAdress);
}

bool ZmqSocket::connect(const char* szSvrIp, int port)
{
    _context = zmq_ctx_new();
    if (!_context)
    {
        LogMsg(Error, "create context errro.");
        return false;
    }

    _socket = zmq_socket(_context, _type);
    if (!_socket)
    {
       LogMsg(Error, "create socket failed! error: %s", zmq_strerror(zmq_errno()));
       return false;
    }

    //int ret = zmq_setsockopt(_socket, ZMQ_SUBSCRIBE, "0", 0);
    _ip = std::string(szSvrIp);
    _port = port;

    std::string str = createAddress(_ip.c_str(),port);
    int ret = zmq_connect(_socket, str.c_str());
    if (ret < 0)
    {
        LogMsg(Error, "connect failed! address : %s, error: %s", str.c_str(), zmq_strerror(zmq_errno()));
        return false;
    }

    return true;
}

int ZmqSocket::bind(const char* szSvrIp, int port)
{
    _context = zmq_ctx_new();
    if (!_context)
    {
        LogMsg(Error, "create context errro.");
        return false;
    }

    _socket = zmq_socket(_context, _type);
    if (!_socket)
    {
       LogMsg(Error, "create socket failed! error: %s", zmq_strerror(zmq_errno()));
       return false;
    }

    _ip = std::string(szSvrIp);
    _port = port;

    std::string str = createAddress(_ip.c_str(),port);
    int ret = zmq_connect(_socket, str.c_str());
    if (ret < 0)
    {
        LogMsg(Error, "connect failed! address : %s, error: %s", str.c_str(), zmq_strerror(zmq_errno()));
        return false;
    }

    return true;
}

int ZmqSocket::setSockOpt(int sockOpt, void* val, int len)
{
    return zmq_setsockopt(_socket, sockOpt, val, len);
}

int ZmqSocket::sendData(const Buffer& buf)
{
    int ret = zmq_send(_socket, buf.getBuf(), buf.getLen(), 0);
    if (ret < 0)
    {
        LogMsg(Error, "send data error : %s", zmq_strerror(zmq_errno()));
        return false;
    }

    return ret;
}

int ZmqSocket::sendData(const char* szData, int len)
{
    int ret = zmq_send(_socket, szData, len, 0);
    if (ret < 0)
    {
       LogMsg(Error, "send data error : %s", zmq_strerror(zmq_errno()));
       return false;
    }

    return ret;
}

int ZmqSocket::recvData(Buffer& buf)
{
    std::vector<char> data_buffer;
    zmq_msg_t msg;
    zmq_msg_init(&msg);
    int ret = zmq_msg_recv(&msg, _socket, 0);
    if (ret >= 0 )
    {
        void * pbuffer = zmq_msg_data(&msg);
        size_t len = zmq_msg_size(&msg);
        for (size_t i = 0; i < len; i++)
        {
            data_buffer.push_back(((char *)pbuffer)[i]);
        }
    }
    else
    {
        LogMsg(Error, "receive failed: %s",zmq_strerror(zmq_errno()));
        return -1;
    }

    zmq_msg_close(&msg);

    while (true)
    {
        long more;
        size_t more_len = sizeof(more);
        int ret = zmq_getsockopt(_socket, ZMQ_RCVMORE, &more, &more_len);
        if (ret < 0)
        {
            LogMsg(Error, "Get More data failed! error is : %s", zmq_strerror(zmq_errno()));
            return -1;
        }
        if (!more)
        {
           // data_buffer.push_back(0);
            break;
        }

        zmq_msg_t part;
        zmq_msg_init(&part);
        zmq_msg_recv(&part, _socket, 0);
        void * pbuffer = zmq_msg_data(&part);
        size_t len = zmq_msg_size(&part);

        data_buffer.push_back('!');//seperator
        data_buffer.push_back('@');
        data_buffer.push_back('#');
        for (size_t i = 0; i < len; i++)
        {
            data_buffer.push_back(((char *)pbuffer)[i]);
        }

        zmq_msg_close(&part);
    }
    //LogMsg(Debug, "recv: %s", buf.getBuf());

    buf.setBuf(&data_buffer[0], data_buffer.size());
    return buf.getLen();
}

int ZmqSocket::select(int evt, int timeout)
{
    zmq_pollitem_t items[1];
    items[0].socket = _socket;
    items[0].events = evt;

    int rc = zmq_poll(items, 1, timeout);

    return rc;//0：超时; >0:有消息
}

