#ifndef CVSDATATREEVIEW_H
#define CVSDATATREEVIEW_H

#include <QTreeView>

class CVSDataTreeView : public QTreeView
{
    Q_OBJECT

public:
    explicit CVSDataTreeView(QWidget *parent = 0);
    ~CVSDataTreeView();

    int loadData();

};

#endif
