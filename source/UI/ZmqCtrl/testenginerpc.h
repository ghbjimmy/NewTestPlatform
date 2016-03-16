#ifndef TESTENGINERPC_H
#define TESTENGINERPC_H

class ZmqSocket;

class TestEngineRpc
{
public:
    TestEngineRpc(int index);
    ~TestEngineRpc();

    bool init(const char *subIp, int subPort, const char *reqIp, int reqPort);
    bool start();
    void stop();

private:
    int _index;
    ZmqSocket* _subSocket;

};

#endif // TESTENGINERPC_H
