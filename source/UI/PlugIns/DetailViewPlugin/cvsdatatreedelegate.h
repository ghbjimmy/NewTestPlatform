#ifndef CVSDATATREEDELEGATE_H
#define CVSDATATREEDELEGATE_H

#include <QStyledItemDelegate>

class CVSDataTreeDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit CVSDataTreeDelegate(QWidget *parent = 0);
    ~CVSDataTreeDelegate();

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

private:

};

#endif
