#ifndef STATEMACHINERPC_H
#define STATEMACHINERPC_H


class ZmqSocket;

class StateMachineRpc
{
public:
    StateMachineRpc();
    ~StateMachineRpc();

    bool init(const char *subIp, int subPort, const char *reqIp, int reqPort);
    bool start();
    void stop();

private:
    ZmqSocket* _subSocket;
};

#endif // STATEMACHINERPC_H
