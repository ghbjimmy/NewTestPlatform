#include "DataTreeNode.h"

DataTreeNode::DataTreeNode()
{
    _parent = NULL;
}

DataTreeNode::~DataTreeNode()
{
    if (_parent != NULL)
    {
        delete _parent;
        _parent = NULL;
    }

    foreach (DataTreeNode* item , _children)
    {
        if (NULL != item)
        {
            delete item;
            item = NULL;
        }
    }

    _children.clear();
}

DataTreeNode* DataTreeNode::parent()
{
    return _parent;
}


DataTreeNode& DataTreeNode::setParent(DataTreeNode* value)
{
    //判断新旧父节点是否相等
    if (_parent != value)
    {
        //判断新父节点是否为NULL
        if (value != NULL)
        {
            if (_parent != NULL)
            {
                //判断原父节点的孩子列表中是否存在当前节点
                if (_parent->children().indexOf(this, 0) >= 0)
                {
                    //原父节点和当前节点解除关系
                    _parent->children().removeOne(this);
                }
            }

            //设置新的父节点
            _parent = value;
            //和新父节点进行关联
            if (_parent->children().indexOf(this, 0) < 0)
            {
                _parent->children().append(this);
            }
        }
        else
        {
            //判断原来的父节点是否为NULL
            if (_parent != NULL)
            {
                //原父节点和当前节点解除关系
                _parent->children().removeOne(this);
            }

            //设置新父节点为NULL
            _parent = NULL;
        }
    }

    return *this;
}

QList<DataTreeNode*>& DataTreeNode::children()
{
    return _children;
}

void DataTreeNode::insertChild(int index, DataTreeNode* child)
{
    int size = _children.size();
    if (index > size)
    {
        return;
    }

    //判断child原来的父节点是否为null，如果不为需要解除原来的关系先
    if (child->_parent != NULL)
    {
        child->setParent(NULL);
    }

    //设置当前的childe的父节点为当前节点
    child->_parent = this;

    //插入到孩子列表中
    _children.insert(index, child);
}

DataTreeNode* DataTreeNode::removeChild(int index)
{
    int size = _children.size();
    if (index > size)
    {
        return NULL;
    }

    //找到当前的节点
    DataTreeNode* node = _children.at(index);

    //从孩子节点列表中删除当前节点
    _children.removeAt(index);

    //对当前节点的父节点设置为空
    node->_parent = NULL;

    return node;
}

//获取节点下所有的子孙节点
void DataTreeNode::allChildNode(DataTreeNode* parent, QList<DataTreeNode*>& children)
{
    if (NULL == parent)
        return;

    QList<DataTreeNode*> childs = parent->children();
    for (int i = 0; i < childs.size(); ++i)
    {
        DataTreeNode* node = childs.at(i);
        if (!node->children().isEmpty())
        {
            allChildNode(node, children);
        }

        children.push_back(node);
    }

}

//获取节点所有的父节点
void DataTreeNode::allParentNode(DataTreeNode* child, QList<DataTreeNode*>& parents)
{
    if (NULL == child)
        return;

    DataTreeNode* parent = child->parent();
    while (parent)
    {
        parents.push_back(parent);
        parent = parent->parent();
    }
}
