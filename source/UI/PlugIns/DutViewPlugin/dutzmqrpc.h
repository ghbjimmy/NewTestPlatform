#ifndef DUTZMQRPC_H
#define DUTZMQRPC_H

#include <QString>

class ZmqSocket;
namespace std
{
    class thread;
}


class DutZmqRpc
{
public:
    DutZmqRpc(int index);
    ~DutZmqRpc();

    bool init(const QString&  pubIp, int pubPort, const QString&  reqIp, int reqPort);

    bool start();
    void stop();
    inline bool isStop() const {return _isStop;}
    inline ZmqSocket* getSubSocket() const {return _subSocket;}

private:
    int _index;
    ZmqSocket* _subSocket;
    ZmqSocket* _reqSocket;
    bool _isStop;
    std::thread* _subThread;
};

#endif // DUTZMQRPC_H
