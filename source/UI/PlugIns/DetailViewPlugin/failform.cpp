#include "failform.h"
#include <QHBoxLayout>
#include <QLabel>

#include "cvsdataadapter.h"
#include "cvsdatatreemodel.h"
#include "cvsdatatreenode.h"

FailForm::FailForm(QWidget *parent) :QTreeView(parent)
{
    CVSDataAdapter* mgr = new CVSDataAdapter();
   // mgr->init(NULL, 0);

    CVSDataTreeModel* model = new CVSDataTreeModel();
    model->setRootNode(mgr->getRootNode());
    this->setModel(model);
    this->expandAll();
}

FailForm::~FailForm()
{

}


