#include "fctviewform.h"

FctViewForm::FctViewForm(IPlugin *plugIn, QWidget *parent) : IModuleForm(plugIn, parent)
{

}

FctViewForm::~FctViewForm()
{

}

bool FctViewForm::init()
{
    return true;
}

void FctViewForm::clear()
{

}

QVector<QAction*> FctViewForm::getActions()
{
    return QVector<QAction*>();
}
