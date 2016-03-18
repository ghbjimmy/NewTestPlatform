#ifndef BASERPC_H
#define BASERPC_H

#include <QObject>

class ZmqSocket;
namespace std
{
    class thread;
}

//通信对象基类，主要处理心跳流程，其它消息由子类处理

class BaseRpc : public QObject
{
    Q_OBJECT

public:
    BaseRpc(int index);
    virtual ~BaseRpc();

    bool init(const QString& pubIp, int pubPort, const QString& reqIp, int reqPort);
    bool start();
    void stop();
    inline bool isStop() const {return _isStop;}
    inline ZmqSocket* getSubSocket() const {return _subSocket;}

    //由定时线程设置是否活着
    void setAlive(bool flag);
    bool isAlive();

    //心跳通知
    void aliveNoity(bool isShow);

    virtual bool procSubRecvMsg(const QString& msg);

signals:
    void isAliveSignal(int id, bool isAlive, bool isShow);

protected:
    int _index;
    int _aliveFlag;

    ZmqSocket* _subSocket;
    ZmqSocket* _reqSocket;
    bool _isStop;
    std::thread* _subThread;
};

#endif // BASERPC_H
