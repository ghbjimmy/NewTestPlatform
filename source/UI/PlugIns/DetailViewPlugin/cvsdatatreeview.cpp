
#include "cvsdatatreeview.h"
#include "cvsdatatreemodel.h"
#include "cvsdatatreenode.h"
#include "cvsdatatreedelegate.h"

#include <QContextMenuEvent>
#include <QMenu>

static const QString& TESTING = "Testing";

const int COL_UNIT = 4;
const int COL_LOWER = 5;
const int COL_UPPER = 6;

CVSDataTreeView::CVSDataTreeView(QWidget *parent) : QTreeView(parent)
{
    //CVSDataTreeDelegate* delegate = new CVSDataTreeDelegate(this);
    //this->setItemDelegate(delegate);

     _model = new CVSDataTreeModel();
      this->setModel(_model);

     this->setColumnHidden(COL_UNIT, true);
     this->setColumnHidden(COL_LOWER, true);
     this->setColumnHidden(COL_UPPER, true);
     createPopupMenu();
}

CVSDataTreeView::~CVSDataTreeView()
{

}

void CVSDataTreeView::setRootNode(CVSDataTreeNode* rootNode)
{
    _model->setRootNode(rootNode);
    this->expandAll();
}

void setText(int index, const QString& text, TDetailViewItem *viewItem)
{
    if (0 == index)
    {
        viewItem->uut1 = text;
    }
    else if (1 == index)
    {
        viewItem->uut2 = text;
    }
    else if (2 == index)
    {
        viewItem->uut3 = text;
    }
    else if (3 == index)
    {
        viewItem->uut4 = text;
    }
    else if (4 == index)
    {
        viewItem->uut5 = text;
    }
    else if (5 == index)
    {
        viewItem->uut6 = text;
    }
}

void CVSDataTreeView::procItemStart(int index, const TItemStart& data, TDetailViewItem *viewItem)
{
    setText(index, TESTING, viewItem);
    this->update();
}

void CVSDataTreeView::procItemEnd(int index, const TItemEnd& itemEnd, TDetailViewItem* viewItem)
{
    QString text = "";
    if (itemEnd.result == -1)
    {
        text = itemEnd.error;
    }
    else if (itemEnd.result == 1)
    {
        if (itemEnd.value.isEmpty())
            text = "PASS";
        else
            text = itemEnd.value;
    }
    else if (itemEnd.result == 0)//失败
    {
        text = itemEnd.value;
    }
    else
    {
        text = "SKIPED"; //skip resutl = 2;
    }

    setText(index, text, viewItem);
    this->update();
}

void CVSDataTreeView::createPopupMenu()
{
    _popUpMenu = new QMenu(this);
    QAction* showUnit = _popUpMenu->addAction("Show Unit Col");
    QAction* showLower = _popUpMenu->addAction("Show Lower Col");
    QAction* showUpper = _popUpMenu->addAction("Show Upper Col");
    showUnit->setCheckable(true);
    showLower->setCheckable(true);
    showUpper->setCheckable(true);

    connect(showUnit,SIGNAL(triggered(bool)),this,SLOT(onMenuAction(bool)));
    connect(showLower,SIGNAL(triggered(bool)),this,SLOT(onMenuAction(bool)));
    connect(showUpper,SIGNAL(triggered(bool)),this,SLOT(onMenuAction(bool)));
}

void CVSDataTreeView::onMenuAction(bool ischecked)
{
    QAction* action = static_cast<QAction*>(sender());
    if (NULL == action)
        return;
    if (action->text() == "Show Unit Col")
    {
       this->setColumnHidden(COL_UNIT, !ischecked);
    }
    else if (action->text() == "Show Lower Col")
    {
        this->setColumnHidden(COL_LOWER, !ischecked);
    }
    else if (action->text() == "Show Upper Col")
    {
        this->setColumnHidden(COL_UPPER, !ischecked);
    }
}

void CVSDataTreeView::contextMenuEvent(QContextMenuEvent* event)
{
    _popUpMenu->exec(QCursor::pos());
    event->accept();
}
