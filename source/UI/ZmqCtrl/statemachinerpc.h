#ifndef STATEMACHINERPC_H
#define STATEMACHINERPC_H

#include <QString>
class ZmqSocket;

class StateMachineRpc
{
public:
    StateMachineRpc();
    ~StateMachineRpc();

    bool init(const QString& pubIp, int pubPort, const QString& reqIp, int reqPort);
    bool start();
    void stop();

private:
    ZmqSocket* _subSocket;
};

#endif // STATEMACHINERPC_H
