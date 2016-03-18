#ifndef STATEMACHINERPC_H
#define STATEMACHINERPC_H

#include "baserpc.h"

class StateMachineRpc : public BaseRpc
{
public:
    StateMachineRpc(int index);
    ~StateMachineRpc();

    bool procSubRecvMsg(const QString& msg);

};

#endif // STATEMACHINERPC_H
