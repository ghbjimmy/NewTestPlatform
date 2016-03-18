#include "testenginerpc.h"
#include "qlog.h"


TestEngineRpc::TestEngineRpc(int index) : BaseRpc(index)
{

}

TestEngineRpc::~TestEngineRpc()
{

}

bool TestEngineRpc::procSubRecvMsg(const QString& msg)
{
    return true;
}
