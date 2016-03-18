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
    inline ZmqSocket* getSubSocket() const {return _subSocket;}

    //加载csv
    bool loadProfile(const QString& csvFilePath);

    //获取csv内存
    bool getContent(QVector<QString>& items);

    //由定时线程设置是否活着
    void setAlive(bool flag);
    bool isAlive();

    //获取进度数据
    bool getProcData();

    //心跳通知
    void aliveNoity(bool isShow);

    //处理item_start
    bool procItemStart(const QString& msg);

    //处理item_end
    bool procItemEnd(const QString& msg);


signals:
    void isAliveSignal(int id, bool isAlive, bool isShow);

private:
    int _index;
    int _aliveFlag;

    ZmqSocket* _subSocket;
    ZmqSocket* _reqSocket;
    bool _isStop;
    std::thread* _subThread;


};

#endif // SEQUENCERRPC_H
