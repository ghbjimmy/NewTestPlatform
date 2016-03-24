#ifndef SEQUENCERRPC_H
#define SEQUENCERRPC_H


#include <QVector>
#include "baserpc.h"

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
    void eventSignal(int index, int evt, const QString& item);

private:

};

#endif // SEQUENCERRPC_H
