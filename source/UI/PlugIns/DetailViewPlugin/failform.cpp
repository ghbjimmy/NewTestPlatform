#include "failform.h"
#include "cvsdatatreemodel.h"
#include "cvsdatatreenode.h"
#include "cvsdatatreedelegate.h"
#include "structdefine.h"

FailForm::FailForm(QWidget *parent) :QTreeView(parent)
{
    //CVSDataTreeDelegate* delegate = new CVSDataTreeDelegate(this);
    //this->setItemDelegate(delegate);

    _model = new CVSDataTreeModel();
    _rootNode = new CVSDataTreeNode();
    _model->setRootNode(_rootNode);
    this->setModel(_model);
}

FailForm::~FailForm()
{

}

void FailForm::procItemEnd(int index, const TItemEnd& itemEnd, TDetailViewItem* viewItem)
{
    if (itemEnd.result == "-1")
    {
        TDetailViewItem* selfItem = new TDetailViewItem();
        *selfItem = *viewItem;

        CVSDataTreeNode* childNode = new CVSDataTreeNode();
        childNode->setData(selfItem, sizeof(TDetailViewItem));
        childNode->setParent(_rootNode);
    }
}


