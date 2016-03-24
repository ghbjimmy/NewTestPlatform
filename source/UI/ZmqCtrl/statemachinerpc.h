#ifndef STATEMACHINERPC_H
#define STATEMACHINERPC_H

#include "baserpc.h"

class StateMachineRpc : public BaseRpc
{
public:
    StateMachineRpc(int index);
    ~StateMachineRpc();

    bool procSubRecvMsg(const QString& msg);

    bool startTest();
    bool stopTest();

private:
    bool execFunc(const QString& func);

};

#endif // STATEMACHINERPC_H
