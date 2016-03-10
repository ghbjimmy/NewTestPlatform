#ifndef DATATREEMODEL_H
#define DATATREEMODEL_H

#include <QAbstractItemModel>
#include "datatreenode.h"

class DataTreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    DataTreeModel(QObject *parent = 0);
    virtual ~DataTreeModel();

    //设置模型数据
    void setRootNode(DataTreeNode *node);

    virtual QModelIndex index(int row, int column, const QModelIndex &parent) const;
    virtual QModelIndex parent(const QModelIndex &child) const;
    virtual int rowCount(const QModelIndex &parent) const;
    virtual int columnCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

protected:
    DataTreeNode* _rootNode;
    DataTreeNode* nodeFromIndex(const QModelIndex &index) const;

};

#endif
