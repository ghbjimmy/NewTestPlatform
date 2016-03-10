#ifndef DATATREENODE_H
#define DATATREENODE_H

/***********************
 树节点基类
***********************/

#include <QList>

class DataTreeNode
{

public:
    DataTreeNode();
    virtual ~DataTreeNode();

    //绑定数据
    virtual void setData(void* data, int len) = 0;

public:
    DataTreeNode* parent();

    DataTreeNode& setParent(DataTreeNode* value);
    QList<DataTreeNode*>& children();
    void insertChild(int index, DataTreeNode* child);
    DataTreeNode* removeChild(int index);

private:
    DataTreeNode* _parent;
    QList<DataTreeNode*> _children;

private:

};

#endif
