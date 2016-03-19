
#include "cvsdatatreeview.h"
#include "cvsdatatreemodel.h"
#include "cvsdatatreenode.h"
#include "cvsdatatreedelegate.h"
#include "structdefine.h"

static const QString& TESTING = "Testing";

CVSDataTreeView::CVSDataTreeView(QWidget *parent) : QTreeView(parent)
{
    //CVSDataTreeDelegate* delegate = new CVSDataTreeDelegate(this);
    //this->setItemDelegate(delegate);

     _model = new CVSDataTreeModel();
      this->setModel(_model);
}

CVSDataTreeView::~CVSDataTreeView()
{

}

void CVSDataTreeView::setRootNode(CVSDataTreeNode* rootNode)
{
    _model->setRootNode(rootNode);
    this->expandAll();

    return;
}

void setText(int index, const QString& text, TDetailViewItem *viewItem)
{
    if (0 == index)
    {
        viewItem->uut1 = text;
    }
    else if (1 == index)
    {
        viewItem->uut2 = text;
    }
    else if (2 == index)
    {
        viewItem->uut3 = text;
    }
    else if (3 == index)
    {
        viewItem->uut4 = text;
    }
    else if (4 == index)
    {
        viewItem->uut5 = text;
    }
    else if (5 == index)
    {
        viewItem->uut6 = text;
    }
}

void CVSDataTreeView::procItemStart(int index, const TItemStart *data, TDetailViewItem *viewItem)
{
    setText(index, TESTING, viewItem);
}

void CVSDataTreeView::procItemEnd(int index, const TItemEnd *itemEnd, TDetailViewItem* viewItem)
{
    QString text = "";
    if (itemEnd->result == "-1")
    {
        text = itemEnd->error;
    }
    else if (itemEnd->result == "pass")
    {
        if (!itemEnd->value.isEmpty())
            text = itemEnd->value;
        else
            text = itemEnd->value;
    }
    else
        text = itemEnd->result;

    setText(index, text, viewItem);
}

