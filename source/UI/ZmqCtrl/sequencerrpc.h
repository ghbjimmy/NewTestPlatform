#ifndef SEQUENCERRPC_H
#define SEQUENCERRPC_H

#include <string>
#include <QVector>

class ZmqSocket;
class TCsvDataItem;

namespace std
{
    class thread;
}

class SequencerRpc
{
public:
    SequencerRpc(int index);
    ~SequencerRpc();

    bool init(const char *subIp, int subPort, const char *reqIp, int reqPort);
    bool start();
    void stop();

    //加载csv
    bool loadProfile(const char* csvFilePath);

    //获取csv内存
    bool getContent(QVector<TCsvDataItem*>& items);

    //检测心跳
    bool isAlive();

    //获取进度数据
    bool getProcData();

    inline ZmqSocket* getSubSocket() const {return _subSocket;}

private:
    int _index;
    ZmqSocket* _subSocket;
    ZmqSocket* _reqSocket;

    std::thread* _subThread;
};

#endif // SEQUENCERRPC_H
