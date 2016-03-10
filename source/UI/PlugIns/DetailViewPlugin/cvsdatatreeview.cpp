
#include "cvsdatatreeview.h"

#include "cvsdataadapter.h"
#include "cvsdatatreemodel.h"
#include "cvsdatatreenode.h"

CVSDataTreeView::CVSDataTreeView(QWidget *parent) :
    QTreeView(parent)
{

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
