
#include "cvsdatatreeview.h"
#include "cvsdataadapter.h"
#include "cvsdatatreemodel.h"
#include "cvsdatatreenode.h"
#include "cvsdatatreedelegate.h"

CVSDataTreeView::CVSDataTreeView(QWidget *parent) : QTreeView(parent)
{
    CVSDataTreeDelegate* delegate = new CVSDataTreeDelegate(this);
    this->setItemDelegate(delegate);
}

CVSDataTreeView::~CVSDataTreeView()
{

}

int CVSDataTreeView::loadData()
{
    CVSDataAdapter* mgr = new CVSDataAdapter();
    mgr->init(NULL, 0);

    CVSDataTreeModel* model = new CVSDataTreeModel();
    model->setRootNode(mgr->getRootNode());
    this->setModel(model);
	this->expandAll();
    return 0;
}
