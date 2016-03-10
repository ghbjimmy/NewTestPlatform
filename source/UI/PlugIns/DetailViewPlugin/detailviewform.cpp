#include "detailviewform.h"
#include <QHBoxLayout>
#include <QLabel>
#include "cvsdatatreeview.h"
#include "progressform.h"
#include "failform.h"

DetailViewForm::DetailViewForm(QWidget *parent) :
    QWidget(parent)
{
    CVSDataTreeView* view = new CVSDataTreeView();
    setupUI();
}

DetailViewForm::~DetailViewForm()
{

}

void DetailViewForm::setupUI()
{
    QHBoxLayout* h1 = new QHBoxLayout();
    mTabWidget = new QTabWidget();
    CVSDataTreeView* treeView = new CVSDataTreeView();
    treeView->loadData();

    FailForm* failForm = new FailForm();

    ProgressForm* progressForm = new ProgressForm();
    mTabWidget->addTab(treeView ,QString("Detail"));
    mTabWidget->addTab(progressForm,QString("Progress"));
    mTabWidget->addTab(failForm,QString("Fail Only"));

    h1->addWidget(mTabWidget);


    this->setLayout(h1);
}
