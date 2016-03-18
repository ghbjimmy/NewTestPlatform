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
    QStyleOptionViewItem opt(option);
    if (node->getUUT1() == "error")
    {
        int i = 0;
        {
            //painter->drawText(opt.rect, Qt::red);
        }

    }
    else
    {
       // painter->fillRect(opt.rect, Qt::green);
    }

    QStyledItemDelegate::paint(painter, opt, index);
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
