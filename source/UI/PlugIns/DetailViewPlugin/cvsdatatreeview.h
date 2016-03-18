#ifndef CVSDATATREEVIEW_H
#define CVSDATATREEVIEW_H

#include <QTreeView>
#include <QVector>

class CVSDataAdapter;

class CVSDataTreeView : public QTreeView
{
    Q_OBJECT

public:
    explicit CVSDataTreeView(QWidget *parent = 0);
    ~CVSDataTreeView();

    bool setData(const QVector<QString>& datas);

    void procItemStart(int index, const QString& data);
    void procItemEnd(int index, const QString& data);

signals:
    void itemFailed(int index, int slotNum, void* viewItem); //通知失败情况

private:
    QVector<QString> _datas;
    CVSDataAdapter* _adapter;
};

#endif
