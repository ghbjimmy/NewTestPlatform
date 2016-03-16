#ifndef SEQUENCERRPC_H
#define SEQUENCERRPC_H

#include <string>

class ZmqSocket;

class SequencerRpc
{
public:
    SequencerRpc();
    ~SequencerRpc();

    bool init();
    bool start(const char *pubIp, int pubPort, const char *reqIp, int reqPort);
    void stop();

    //加载csv，返回结果json串
    bool loadProfile(const char* csvFilePath, std::string& contentJson);

    //检测心跳
    bool isAlive();

    //获取进度数据
    bool getProcData();

private:
    ZmqSocket* _pubSocket;
    ZmqSocket* _reqSocket;
};

#endif // SEQUENCERRPC_H
