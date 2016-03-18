
#include "cvsdatatreeview.h"
#include "cvsdatatreemodel.h"
#include "cvsdatatreenode.h"
#include "cvsdatatreedelegate.h"
#include "cvsdataadapter.h"


CVSDataTreeView::CVSDataTreeView(QWidget *parent) : QTreeView(parent)
{
    CVSDataTreeDelegate* delegate = new CVSDataTreeDelegate(this);
    this->setItemDelegate(delegate);

     _adapter = new CVSDataAdapter();
}

CVSDataTreeView::~CVSDataTreeView()
{

}

bool CVSDataTreeView::setData(const QVector<QString>& datas)
{
    _datas = datas;
    if (!_adapter->convertData(_datas))
        return false;

    CVSDataTreeNode* rootNode = _adapter->getRootNode();
    CVSDataTreeModel* model = new CVSDataTreeModel();
    model->setRootNode(rootNode);
    this->setModel(model);
    this->expandAll();

    return true;
}

void CVSDataTreeView::procItemStart(int index, const QString& data)
{
    TItemStart itemStart;
    if (!_adapter->convertItemStart(data, &itemStart))
        return;
}

void CVSDataTreeView::procItemEnd(int index, const QString& data)
{
    TItemEnd itemEnd;
    if (!_adapter->convertItemEnd(data, &itemEnd))
        return;

}

