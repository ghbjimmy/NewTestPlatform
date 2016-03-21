#ifndef SEQUENCERRPC_H
#define SEQUENCERRPC_H


#include <QVector>
#include "baserpc.h"
#include "structdefine.h"

class SequencerRpc : public BaseRpc
{
    Q_OBJECT
public:
    SequencerRpc(int index);
    ~SequencerRpc();

    //加载csv
    bool loadProfile(const QString& csvFilePath);

    //获取csv内存
    bool getCsvContent(QVector<QString>& items);


    bool procSubRecvMsg(const QString& msg);

signals:
    void itemStartSignal(int index, const QString& item);
    void itemEndSignal(int index, const QString& item);

private:

};

#endif // SEQUENCERRPC_H
