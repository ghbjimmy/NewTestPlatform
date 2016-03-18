#ifndef STATEMACHINERPC_H
#define STATEMACHINERPC_H

#include <QObject>
#include <QString>
class ZmqSocket;

class StateMachineRpc : public QObject
{
    Q_OBJECT
public:
    StateMachineRpc(int index);
    ~StateMachineRpc();

    bool init(const QString& pubIp, int pubPort, const QString& reqIp, int reqPort);
   // bool start();
   // void stop();

  //  inline ZmqSocket* getSubSocket() const {return _subSocket;}

signals:
    void isAliveSignal(int id, bool isAlive, bool isShow);

private:
    ZmqSocket* _subSocket;
};

#endif // STATEMACHINERPC_H
