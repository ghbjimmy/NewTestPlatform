#ifndef CVSDATAADAPTER_H
#define CVSDATAADAPTER_H

#include <QString>
#include <QVector>

class CVSDataTreeNode;

//负责把数据适配
class CVSDataAdapter
{
public:
    CVSDataAdapter();
    ~CVSDataAdapter();

    bool convertData(const QVector<QString>& items);

    inline CVSDataTreeNode* getRootNode() const { return _rootNode; }

private:
    CVSDataTreeNode* _rootNode;
};

#endif
