
#include "cvsdataadapter.h"
#include "cvsdatatreenode.h"
#include "../detailviewsinterface.h"

CVSDataAdapter::CVSDataAdapter()
{
    _rootNode = NULL;
}

CVSDataAdapter::~CVSDataAdapter()
{

}

int CVSDataAdapter::init(void* data, int len)
{
    _rootNode = new CVSDataTreeNode();
    int index = 1;
    for (int i = 0; i < 2; ++i)
    {
        CVSDataTreeNode* groupNode = new CVSDataTreeNode();
        TDetailViewItem item;
		item.index = "Group" + QString::number(i);

        groupNode->setData(&item, sizeof(TDetailViewItem));
		groupNode->setParent(_rootNode);

        for (int j = 0; j < 10; ++j)
        {
            CVSDataTreeNode* dataNode = new CVSDataTreeNode();
            TDetailViewItem item1;
			item1.index = QString::number(index++);
            item1.testKey = "aaa" + QString::number(j + 1);
            dataNode->setData(&item1, sizeof(TDetailViewItem));
            dataNode->setParent(groupNode);
        }
    }

    return 0;
}
