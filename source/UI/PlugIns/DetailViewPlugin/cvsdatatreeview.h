#ifndef CVSDATATREEVIEW_H
#define CVSDATATREEVIEW_H

#include <QTreeView>
#include "structdefine.h"

class CVSDataTreeNode;
class CVSDataTreeModel;
class TDetailViewItem;

class CVSDataTreeView : public QTreeView
{
    Q_OBJECT

public:
    explicit CVSDataTreeView(QWidget *parent = 0);
    ~CVSDataTreeView();

    void setRootNode(CVSDataTreeNode* rootNode);

    void procItemStart(int index, const TItemStart& data, TDetailViewItem* viewItem);
    void procItemEnd(int index, const TItemEnd& itemEnd, TDetailViewItem* viewItem);

signals:
    void itemFailed(int index, int slotNum, void* viewItem); //通知失败情况

private:
    CVSDataTreeModel* _model;
};

#endif
