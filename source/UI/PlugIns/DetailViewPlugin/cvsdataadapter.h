#ifndef CVSDATAADAPTER_H
#define CVSDATAADAPTER_H

#include <QString>
#include <QVector>

#include "structdefine.h"

class CVSDataTreeNode;
class TDetailViewItem;

//负责把数据适配
class CVSDataAdapter
{
public:
    static CVSDataTreeNode* convertData(const QVector<QString>& items, QVector<TDetailViewItem*>& viewItems);
    static TItemStart* convertItemStart(const QString& itemJson);
    static TItemEnd* convertItemEnd(const QString& itemJson);

private:
    CVSDataAdapter();

};

#endif
