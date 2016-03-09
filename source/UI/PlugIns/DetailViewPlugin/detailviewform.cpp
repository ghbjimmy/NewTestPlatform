#include "detailviewform.h"
#include <QHBoxLayout>

DetailViewForm::DetailViewForm(QWidget *parent) :
    QWidget(parent)
{
    SetupUI();
}

DetailViewForm::~DetailViewForm()
{

}

void DetailViewForm::SetupUI()
{
    QHBoxLayout* h1 = new QHBoxLayout();
    mTabWidget = new QTabWidget();
    h1->addWidget(mTabWidget);
    this->setLayout(h1);
}
