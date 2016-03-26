#include "fctviewform.h"
#include "plugin_global.h"

FctViewForm::FctViewForm(IPlugin *plugIn, QWidget *parent) : QWidget(parent)
{
    _plugIn = plugIn;
}

FctViewForm::~FctViewForm()
{

}
