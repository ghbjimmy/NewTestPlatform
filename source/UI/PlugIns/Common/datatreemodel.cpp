#include "datatreemodel.h"

DataTreeModel::DataTreeModel(QObject *parent) : QAbstractItemModel(parent)
{
    _rootNode = NULL;
}

DataTreeModel::~DataTreeModel()
{
    /*if (_rootNode != NULL)
    {
        delete _rootNode;
        _rootNode = NULL;
    }*/
}

void DataTreeModel::setRootNode(DataTreeNode *node)
{
    if (_rootNode != NULL)
        delete _rootNode;
    _rootNode = node;
    //reset();
}

QModelIndex DataTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!_rootNode || row < 0 || column < 0)
        return QModelIndex();

    DataTreeNode *parentNode = nodeFromIndex(parent);
    DataTreeNode *childNode = parentNode->children().value(row);

    if (!childNode)
        return QModelIndex();

    return createIndex(row, column, childNode);
}

QModelIndex DataTreeModel::parent(const QModelIndex &child) const
{
    DataTreeNode *node = nodeFromIndex(child);
    if (!node)
        return QModelIndex();

    DataTreeNode *parentNode = node->parent();
    if (!parentNode)
        return QModelIndex();

    DataTreeNode *grandparentNode = parentNode->parent();
    if (!grandparentNode)
        return QModelIndex();

    int row = grandparentNode->children().indexOf(parentNode);
    return createIndex(row, 0, parentNode);
}

int DataTreeModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;

    DataTreeNode *parentNode = nodeFromIndex(parent);
    if (!parentNode)
        return 0;

    return parentNode->children().count();
}

int DataTreeModel::columnCount(const QModelIndex &parent) const
{
     return 0;
}

QVariant DataTreeModel::data(const QModelIndex &index, int role) const
{
    return QVariant();
}

Qt::ItemFlags DataTreeModel::flags(const QModelIndex &index) const
{
   return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

DataTreeNode* DataTreeModel::nodeFromIndex(const QModelIndex &index) const
{
    if (index.isValid())
    {
        return static_cast<DataTreeNode *>(index.internalPointer());
    }
    else
    {
        return _rootNode;
    }
}


QVariant DataTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	return QVariant();
}
