#include <QMap>

#include "cvsdataadapter.h"
#include "cvsdatatreenode.h"
#include "detailviewsinterface.h"
#include "structdefine.h"

CVSDataAdapter::CVSDataAdapter()
{
    _rootNode = NULL;
}

CVSDataAdapter::~CVSDataAdapter()
{

}


static bool parseCsvItem(const QString& str, TCsvDataItem* item)
{
    QString data = str.mid(1, str.size() - 2);
    QStringList list = data.split(",");
    for (int i = 0; i < list.size(); ++i)
    {
        QStringList itemList = list[i].split(":");
        if (itemList.size() != 2)
            return false;

        const QString& sFirst = itemList[0];
        const QString& sSecond = itemList[1].remove("'");
        if (sFirst.contains("FUNCTION"))
        {
            item->function = sSecond;
        }
        else if (sFirst.contains("PARAM1"))
        {
            item->param1 = sSecond;
        }
        else if (sFirst.contains("GROUP"))
        {
            item->group = sSecond;
        }
        else if (sFirst.contains("DESCRIPTION"))
        {
            item->desc = sSecond;
        }
        else if (sFirst.contains("VAL"))
        {
            item->val = sSecond;
        }
        else if (sFirst.contains("HIGH"))
        {
            item->high = sSecond;
        }
        else if (sFirst.contains("TIMEOUT"))
        {
            item->timeout = sSecond;
        }
        else if (sFirst.contains("PARAM2"))
        {
            item->param2 = sSecond;
        }
        else if (sFirst.contains("KEY"))
        {
            item->key = sSecond;
        }
        else if (sFirst.contains("TID"))
        {
            item->tid = sSecond;
        }
        else if (sFirst.contains("UNIT"))
        {
            item->unit = sSecond;
        }
        else if (sFirst.contains("LOW"))
        {
            item->low = sSecond;
        }
    }

    return true;
}

CVSDataTreeNode* getTreeNodeByGroupName(const QString groupName, QMap<QString, CVSDataTreeNode*>& groupMap)
{
    CVSDataTreeNode* node = NULL;
    QMap<QString, CVSDataTreeNode*>::iterator iter = groupMap.find(groupName);
    if (iter != groupMap.end())
    {
        node = iter.value();
    }

    return node;
}

bool CVSDataAdapter::convertData(const QVector<QString>& items)
{
    QMap<QString, CVSDataTreeNode*> groupMap;
    _rootNode = new CVSDataTreeNode();
    int size = items.size();
    for (int i = 0; i < size; ++i)
    {
        TCsvDataItem cvsItem;
        if (!parseCsvItem(items[i], &cvsItem))
        {
            return false;
        }


        CVSDataTreeNode* parentNode = getTreeNodeByGroupName(cvsItem.group, groupMap);
        if (parentNode == NULL)
        {
            parentNode = new CVSDataTreeNode();
            TDetailViewItem* parentItem = new TDetailViewItem();
            parentItem->index = cvsItem.group;
            parentNode->setData(parentItem, sizeof(TDetailViewItem));

            groupMap[cvsItem.group] = parentNode;
            parentNode->setParent(_rootNode);
        }

        CVSDataTreeNode* childNode = new CVSDataTreeNode();
        TDetailViewItem* childItem = new TDetailViewItem();

        childItem->index = QString::number(i + 1);
        childItem->testKey = cvsItem.tid;
        childItem->destcription = cvsItem.desc;
        childItem->lower = cvsItem.low;
        childItem->upper = cvsItem.high;
        childItem->unit = cvsItem.unit;
        childItem->time = cvsItem.timeout;

        childNode->setData(childItem, sizeof(TDetailViewItem));
        childNode->setParent(parentNode);
    }

    return true;
}


