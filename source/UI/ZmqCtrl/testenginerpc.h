#ifndef TESTENGINERPC_H
#define TESTENGINERPC_H

#include <QString>
#include <QObject>

class ZmqSocket;

namespace std
{
    class thread;
}

class TestEngineRpc : public QObject
{
    Q_OBJECT

public:
    TestEngineRpc(int index);
    ~TestEngineRpc();

    bool init(const QString& pubIp, int pubPort);
    bool start();
    void stop();
    inline bool isStop() const {return _isStop;}


    //由定时线程设置是否活着
    void setAlive(bool flag);
    bool isAlive();
    void aliveNoity(bool isShow);

    inline ZmqSocket* getSubSocket() const {return _subSocket;}

signals:
    void isAliveSignal(int id, bool isAlive, bool isShow);

private:
    int _index;
    ZmqSocket* _subSocket;
    std::thread* _subThread;
    int _aliveFlag;
    bool _isStop;
};

#endif // TESTENGINERPC_H
