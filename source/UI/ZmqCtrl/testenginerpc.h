#ifndef TESTENGINERPC_H
#define TESTENGINERPC_H

#include "baserpc.h"

class TestEngineRpc : public BaseRpc
{
public:
    TestEngineRpc(int index);
    ~TestEngineRpc();

    bool procSubRecvMsg(const QString& msg);
};

#endif // TESTENGINERPC_H
