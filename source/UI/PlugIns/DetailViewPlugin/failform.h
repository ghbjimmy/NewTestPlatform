#ifndef FAILFORM_H
#define FAILFORM_H

#include <QTreeView>
#include "structdefine.h"

class TDetailViewItem;
class CVSDataTreeModel;
class CVSDataTreeNode;

class FailForm : public QTreeView
{
    Q_OBJECT

public:
    explicit FailForm(QWidget *parent = 0);
    ~FailForm();

    void procItemEnd(int index, const TItemEnd& itemEnd, TDetailViewItem* viewItem);

private:
    CVSDataTreeModel* _model;
    CVSDataTreeNode* _rootNode;
};

#endif // DETAILVIEWFORM_H
