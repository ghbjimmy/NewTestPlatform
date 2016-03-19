#include <QPainter>

#include "cvsdatatreedelegate.h"
#include "cvsdatatreenode.h"

CVSDataTreeDelegate::CVSDataTreeDelegate(QWidget *parent) : QStyledItemDelegate(parent)
{

}

CVSDataTreeDelegate::~CVSDataTreeDelegate()
{

}

void CVSDataTreeDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    int row = index.row();
    int column = index.column();

    CVSDataTreeNode* node = static_cast<CVSDataTreeNode*>(index.internalPointer());
    if (node == NULL)
        return;

    QStyledItemDelegate::paint(painter, option, index);
}


QWidget* CVSDataTreeDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return NULL;
}

void CVSDataTreeDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
}


void CVSDataTreeDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
}
