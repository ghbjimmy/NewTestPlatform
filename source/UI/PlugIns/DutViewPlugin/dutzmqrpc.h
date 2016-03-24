#ifndef DUTZMQRPC_H
#define DUTZMQRPC_H

#include <QString>
#include <QObject>
#include <thread>

class ZmqSocket;

class DutZmqRpc : public QObject
{
    Q_OBJECT
public:
    DutZmqRpc(int index);
    ~DutZmqRpc();

    bool init(const QString&  pubIp, int pubPort, const QString&  reqIp, int reqPort);

    bool start();
    void stop();
    inline bool isStop() const {return _isStop;}
    inline ZmqSocket* getSubSocket() const {return _subSocket;}


    bool sendCommand(const QString& sendMsg, QString& recvMsg);

    bool procSubRecvMsg(const QString& msg);

signals:
    void dutMsgSignal(int index, const QString& msg);

private:
    int _index;
    ZmqSocket* _subSocket;
    ZmqSocket* _reqSocket;
    bool _isStop;
    std::thread* _subThread;
};

#endif // DUTZMQRPC_H
