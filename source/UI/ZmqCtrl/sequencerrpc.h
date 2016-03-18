#ifndef SEQUENCERRPC_H
#define SEQUENCERRPC_H


#include <QVector>
#include "baserpc.h"

class SequencerRpc : public BaseRpc
{
public:
    SequencerRpc(int index);
    ~SequencerRpc();

    //加载csv
    bool loadProfile(const QString& csvFilePath);

    //获取csv内存
    bool getContent(QVector<QString>& items);

    //获取进度数据
    bool getProcData();

    //处理item_start
    bool procItemStart(const QString& msg);

    //处理item_end
    bool procItemEnd(const QString& msg);

    bool procSubRecvMsg(const QString& msg);

};

#endif // SEQUENCERRPC_H
