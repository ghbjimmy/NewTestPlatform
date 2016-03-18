#ifndef SEQUENCERRPC_H
#define SEQUENCERRPC_H

#include <QObject>
#include <QVector>

class ZmqSocket;

namespace std
{
    class thread;
}

class SequencerRpc : public QObject
{
    Q_OBJECT

public:
    SequencerRpc(int index);
    ~SequencerRpc();

    bool init(const QString& pubIp, int pubPort, const QString& reqIp, int reqPort);
    bool start();
    void stop();
    inline bool isStop() const {return _isStop;}
    //加载csv
    bool loadProfile(const QString& csvFilePath);

    //获取csv内存
    bool getContent(QVector<QString>& items);

    //由定时线程设置是否活着
    void setAlive(bool flag);
    bool isAlive();
    //获取进度数据
    bool getProcData();

    void aliveNoity(bool isShow);

    inline ZmqSocket* getSubSocket() const {return _subSocket;}

signals:
    void isAliveSignal(int id, bool isAlive, bool isShow);

private:
    int _index;
    int _aliveFlag;

    ZmqSocket* _subSocket;
    ZmqSocket* _reqSocket;

    std::thread* _subThread;

    bool _isStop;
};

#endif // SEQUENCERRPC_H
