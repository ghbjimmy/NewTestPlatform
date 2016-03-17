
#include "cvsdatatreeview.h"
#include "cvsdatatreemodel.h"
#include "cvsdatatreenode.h"
#include "cvsdatatreedelegate.h"
#include "cvsdataadapter.h"


CVSDataTreeView::CVSDataTreeView(QWidget *parent) : QTreeView(parent)
{
    CVSDataTreeDelegate* delegate = new CVSDataTreeDelegate(this);
    this->setItemDelegate(delegate);
}

CVSDataTreeView::~CVSDataTreeView()
{

}

bool CVSDataTreeView::setData(const QVector<QString>& datas)
{
    _datas = datas;
    _adapter = new CVSDataAdapter();
    if (!_adapter->convertData(_datas))
        return false;

    CVSDataTreeNode* rootNode = _adapter->getRootNode();
    CVSDataTreeModel* model = new CVSDataTreeModel();
    model->setRootNode(rootNode);
    this->setModel(model);
    this->expandAll();

    return true;
}

