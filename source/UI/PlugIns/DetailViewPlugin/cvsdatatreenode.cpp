#include "cvsDataTreeNode.h"
#include "detailviewsinterface.h"

CVSDataTreeNode::CVSDataTreeNode() : DataTreeNode()
{

}

CVSDataTreeNode::~CVSDataTreeNode()
{

}

void CVSDataTreeNode::setData(void* data, int len)
{
    if (data == NULL || len != sizeof(TDetailViewItem))
    {
        throw "parameter error!";
        return;
    }

    _viewItem = static_cast<TDetailViewItem*>(data);
}
