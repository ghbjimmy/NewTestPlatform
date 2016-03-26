#include "failform.h"
#include "cvsdatatreemodel.h"
#include "cvsdatatreenode.h"
#include "cvsdatatreedelegate.h"
#include "structdefine.h"

const int COL_UNIT = 4;
const int COL_LOWER = 5;
const int COL_UPPER = 6;

FailForm::FailForm(QWidget *parent) :QTreeView(parent)
{
    //CVSDataTreeDelegate* delegate = new CVSDataTreeDelegate(this);
    //this->setItemDelegate(delegate);

    _model = new CVSDataTreeModel();
    _rootNode = new CVSDataTreeNode();
    _model->setRootNode(_rootNode);
    this->setModel(_model);

    this->setColumnHidden(COL_UNIT, true);
    this->setColumnHidden(COL_LOWER, true);
    this->setColumnHidden(COL_UPPER, true);
}

FailForm::~FailForm()
{

}

void FailForm::procItemEnd(int index, const TItemEnd& itemEnd, TDetailViewItem* viewItem)
{
    if (itemEnd.result == 0 || itemEnd.result == -1)
    {
        TDetailViewItem* selfItem = new TDetailViewItem();
        *selfItem = *viewItem;

        CVSDataTreeNode* childNode = new CVSDataTreeNode();
        childNode->setData(selfItem, sizeof(TDetailViewItem));
        childNode->setParent(_rootNode);
    }
}


