#ifndef CVSDATATREEMODEL_H
#define CVSDATATREEMODEL_H

#include "datatreemodel.h"


class CVSDataTreeModel : public DataTreeModel
{
    Q_OBJECT
public:
    CVSDataTreeModel(QObject *parent = 0);
    ~CVSDataTreeModel();

    virtual int columnCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

private:

};

#endif
