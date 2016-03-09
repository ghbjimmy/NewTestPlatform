#include "detailviewform.h"
#include <QHBoxLayout>

DetailViewForm::DetailViewForm(QWidget *parent) :
    QWidget(parent)
{
    SetUpUi();
}

DetailViewForm::~DetailViewForm()
{

}

void DetailViewForm::SetUpUi()
{
    QHBoxLayout* h1 = new QHBoxLayout();
    mTabWidget = new QTabWidget();
    h1->addWidget(mTabWidget);
    this->setLayout(h1);
}
