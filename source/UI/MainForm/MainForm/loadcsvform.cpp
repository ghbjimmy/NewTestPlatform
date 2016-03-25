#include "loadcsvform.h"
#include "util.h"
#include <QProgressBar>
#include <QListWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

LoadCsvForm::LoadCsvForm(QWidget *parent) : QDialog(parent)
{
    setupUI();
}

LoadCsvForm::~LoadCsvForm()
{

}

void LoadCsvForm::setupUI()
{
    QLabel* titleLbl = new QLabel("Loading default CSV File : xxxxxx.csv");
    UIUtil::setFontSize(titleLbl, -1, true);
    UIUtil::setTextColor(titleLbl, Qt::blue);
    _bar = new QProgressBar();
    _bar->setRange(0, 6);
    _listWgt = new QListWidget();

    QVBoxLayout* v1 = new QVBoxLayout();
    v1->addWidget(titleLbl);
    v1->addWidget(_bar);
    v1->addWidget(_listWgt, 1);
    this->setLayout(v1);

    this->setFixedSize(480, 200);
}
