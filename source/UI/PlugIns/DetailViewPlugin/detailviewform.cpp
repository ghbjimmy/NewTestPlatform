#include "detailviewform.h"
#include <QHBoxLayout>
#include <QLabel>
#include "cvsdatatreeview.h"

DetailViewForm::DetailViewForm(QWidget *parent) :
    QWidget(parent)
{
    CVSDataTreeView* view = new CVSDataTreeView();
    SetupUI();
}

DetailViewForm::~DetailViewForm()
{

}

void DetailViewForm::SetupUI()
{
    QHBoxLayout* h1 = new QHBoxLayout();
    mTabWidget = new QTabWidget();
    CVSDataTreeView* treeView = new CVSDataTreeView();
    treeView->loadData();

    mTabWidget->addTab(treeView ,QString("Detail"));
    mTabWidget->addTab(new QLabel("Progress"),QString("Progress"));
    mTabWidget->addTab(new QLabel("Fail Only"),QString("Fail Only"));

    h1->addWidget(mTabWidget);


    this->setLayout(h1);
}
