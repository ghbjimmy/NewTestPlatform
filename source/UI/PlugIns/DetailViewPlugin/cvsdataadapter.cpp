#include <QMap>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "cvsdataadapter.h"
#include "cvsdatatreenode.h"
#include "detailviewsinterface.h"
#include "qlog.h"


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
        QString sSecond = itemList[1].remove("'");
        sSecond = sSecond.trimmed();
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

CVSDataTreeNode* CVSDataAdapter::convertData(const QVector<QString>& items, QVector<TDetailViewItem*>& viewItems)
{
    QMap<QString, CVSDataTreeNode*> groupMap;
    CVSDataTreeNode* rootNode = new CVSDataTreeNode();
    int size = items.size();
    for (int i = 0; i < size; ++i)
    {
        TCsvDataItem cvsItem;
        if (!parseCsvItem(items[i], &cvsItem))
        {
            delete rootNode;
            return NULL;
        }


        CVSDataTreeNode* parentNode = getTreeNodeByGroupName(cvsItem.group, groupMap);
        if (parentNode == NULL)
        {
            parentNode = new CVSDataTreeNode();
            TDetailViewItem* parentItem = new TDetailViewItem();
            parentItem->index = cvsItem.group;
            parentItem->group = cvsItem.group;
            parentNode->setData(parentItem, sizeof(TDetailViewItem));

            groupMap[cvsItem.group] = parentNode;
            parentNode->setParent(rootNode);
        }

        CVSDataTreeNode* childNode = new CVSDataTreeNode();
        TDetailViewItem* childItem = new TDetailViewItem();

        childItem->index = QString::number(i + 1);
        childItem->group = cvsItem.group;
        childItem->testKey = cvsItem.tid;
        childItem->destcription = cvsItem.desc;
        childItem->lower = cvsItem.low;
        childItem->upper = cvsItem.high;
        childItem->unit = cvsItem.unit;
        childItem->time = cvsItem.timeout;

        childNode->setData(childItem, sizeof(TDetailViewItem));
        childNode->setParent(parentNode);

        viewItems.push_back(childItem);
    }

    return rootNode;
}

TItemStart* CVSDataAdapter::convertItemStart(const QString& itemJson)
{
    QJsonParseError json_error;
    QJsonDocument document = QJsonDocument::fromJson(itemJson.toUtf8(), &json_error);
    if(json_error.error != QJsonParseError::NoError)
    {
        LogMsg(Error, "Parse ItemStart json failed. %s", itemJson.toStdString().c_str());
        return NULL;
    }

    if (!document.isObject())
    {
        LogMsg(Error, "Parse ItemStar json format is error. %s", itemJson.toStdString().c_str());
        return NULL;
    }

    TItemStart* itemStart = new TItemStart();
    QJsonObject obj = document.object();
    if(obj.contains("group"))
    {
        itemStart->group = obj.value("group").toString();
    }
    if (obj.contains("tid"))
    {
        itemStart->tid = obj.value("tid").toString();
    }
    if (obj.contains("unit"))
    {
        itemStart->unit = obj.value("unit").toString();
    }
    if (obj.contains("low"))
    {
        itemStart->low = obj.value("low").toString();
    }
    if (obj.contains("high"))
    {
        itemStart->high = obj.value("high").toString();
    }
    if (obj.contains("pdca"))
    {
        itemStart->pdca = obj.value("pdca").toString();
    }

    return itemStart;
}

TItemEnd* CVSDataAdapter::convertItemEnd(const QString& itemJson)
{
    QJsonParseError json_error;
    QJsonDocument document = QJsonDocument::fromJson(itemJson.toUtf8(), &json_error);
    if(json_error.error != QJsonParseError::NoError)
    {
        LogMsg(Error, "Parse ItemStart json failed. %s", itemJson.toStdString().c_str());
        return NULL;
    }

    if (!document.isObject())
    {
        LogMsg(Error, "Parse ItemStar json format is error. %s", itemJson.toStdString().c_str());
        return NULL;
    }

    TItemEnd* itemEnd = new TItemEnd();

    QJsonObject obj = document.object();
    if (obj.contains("tid"))
    {
        itemEnd->tid = obj.value("tid").toString();
    }
    if (obj.contains("value"))
    {
        double val = -9999;
        if  (-9999 == (val = obj.value("value").toDouble(-9999)))
        {
            itemEnd->value = obj.value("value").toString();
        }
        else
        {
            itemEnd->value = QString::number(val);
        }
    }
    if (obj.contains("result"))
    {
        int val = -9999;
        if  (-9999 == (val = obj.value("result").toInt(-9999)))
        {
            QString sval = "";
            if ("-9999" == (sval = obj.value("result").toString("-9999")))
            {
                bool bresult = obj.value("result").toBool();
                itemEnd->result = bresult ? 1 : 0;
            }
            else
            {
                itemEnd->result = sval.toInt(); // "-1"
            }
        }
        else
        {
            itemEnd->result = val;
        }
    }
    if (obj.contains("error"))
    {
        itemEnd->error = obj.value("error").toString();
    }
    if (obj.contains("pdca"))
    {
        itemEnd->pdca = obj.value("pdca").toString();
    }

    return itemEnd;
}

