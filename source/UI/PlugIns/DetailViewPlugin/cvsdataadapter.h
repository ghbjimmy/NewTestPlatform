#ifndef CVSDATAADAPTER_H
#define CVSDATAADAPTER_H

#include <QString>
#include <QVector>

#include "structdefine.h"

class CVSDataTreeNode;

//负责把数据适配
class CVSDataAdapter
{
public:
    CVSDataAdapter();
    ~CVSDataAdapter();

    bool convertData(const QVector<QString>& items);
    bool convertItemStart(const QString& itemJson, TItemStart* itemStart);
    bool convertItemEnd(const QString& itemJson, TItemEnd* itemStart);

    inline CVSDataTreeNode* getRootNode() const { return _rootNode; }

private:
    CVSDataTreeNode* _rootNode;
};

#endif
