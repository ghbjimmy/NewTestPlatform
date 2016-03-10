#ifndef CVSDATAADAPTER_H
#define CVSDATAADAPTER_H

#include <QString>

class CVSDataTreeNode;

//负责把数据适配
class CVSDataAdapter
{
public:
    CVSDataAdapter();
    ~CVSDataAdapter();

    int init(void* data, int len);

    inline CVSDataTreeNode* getRootNode() const { return _rootNode; }

private:
    CVSDataTreeNode* _rootNode;
};

#endif
