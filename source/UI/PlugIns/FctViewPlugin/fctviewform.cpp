#include "fctviewform.h"
#include "util.h"

#include <QAction>
#include <QDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>

static const QString& FCT_PANNEL = "FCT Pannel";
FctViewForm::FctViewForm(IPlugin *plugIn, QWidget *parent) : IModuleForm(plugIn, parent)
{
    setupUI();
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
    return QVector<QAction*>() << _showFormAct;
}

void FctViewForm::setupUI()
{
    _showFormAct = UIUtil::createWidgetWithName<QAction>(FCT_PANNEL);
    connect(_showFormAct, SIGNAL(triggered()), this, SLOT(onShowForm()));
}

void FctViewForm::onShowForm()
{
    QDialog* dlg = NULL;
    if (_ownerWgt != NULL)
        dlg = new QDialog(_ownerWgt);
    else
        dlg = new QDialog();

    dlg->setWindowTitle("Fct Debug Pannel");
    QHBoxLayout* h1 = new QHBoxLayout();
    h1->addWidget(this);
    h1->setContentsMargins(0,0,0,0);
    dlg->setLayout(h1);
    dlg->resize(800, 640);
    dlg->show();

    //dlg 内存需要处理，暂时先放着
}

