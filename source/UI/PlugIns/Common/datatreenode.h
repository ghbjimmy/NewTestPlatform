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

    inline int childCount() const {return _children.size();}

    //获取节点下所有的子孙节点
    static void allChildNode(DataTreeNode* parent, QList<DataTreeNode*>& children);

    //获取节点所有的父节点
    static void allParentNode(DataTreeNode* child, QList<DataTreeNode*>& parents);

private:
    DataTreeNode* _parent;
    QList<DataTreeNode*> _children;

private:

};

#endif
