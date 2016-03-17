#ifndef SEQUENCERRPC_H
#define SEQUENCERRPC_H


#include <QVector>

class ZmqSocket;

namespace std
{
    class thread;
}

class SequencerRpc
{
public:
    SequencerRpc(int index);
    ~SequencerRpc();

    bool init(const QString& pubIp, int pubPort, const QString& reqIp, int reqPort);
    bool start();
    void stop();

    //加载csv
    bool loadProfile(const QString& csvFilePath);

    //获取csv内存
    bool getContent(QVector<QString>& items);

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
