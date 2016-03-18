#include "statemachinerpc.h"

StateMachineRpc::StateMachineRpc(int index) : BaseRpc(index)
{

}

StateMachineRpc::~StateMachineRpc()
{

}

bool StateMachineRpc::procSubRecvMsg(const QString& msg)
{
    return true;
}
