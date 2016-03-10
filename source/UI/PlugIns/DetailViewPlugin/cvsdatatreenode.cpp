#include "cvsDataTreeNode.h"
#include "../detailviewsinterface.h"

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

    TDetailViewItem* item = static_cast<TDetailViewItem*>(data);
    _index = item->index;
    _testKey = item->testKey;
    _destcription = item->destcription;
	_time = item->time;
	_lower = item->lower;
	_upper = item->upper;
    _uut1 = item->uut1;
    _uut2 = item->uut2;
    _uut3 = item->uut3;
    _uut4 = item->uut4;
    _uut5 = item->uut5;
    _uut6 = item->uut6;

}
