#ifndef BASERPC_H
#define BASERPC_H

#include <QObject>
#include <thread>

class ZmqSocket;


//通信对象基类，主要处理心跳流程，其它消息由子类处理

enum ERpcState
{
    unknown = 0,
    Alive,
    Dead
};

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
    ERpcState getAliveState(); // 0 : 未知状态 1：活着 2:死亡

    //心跳通知
    void aliveNoity(bool isShow);

    virtual bool procSubRecvMsg(const QString& msg);

signals:
    void isAliveSignal(int id, bool isAlive, bool isShow);

protected:
    int _index;
    ERpcState _aliveState;

    ZmqSocket* _subSocket;
    ZmqSocket* _reqSocket;
    bool _isStop;
    std::thread* _subThread;
};

#endif // BASERPC_H
