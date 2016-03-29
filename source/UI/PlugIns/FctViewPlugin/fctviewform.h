#ifndef FCTVIEWFORM_H
#define FCTVIEWFORM_H

#include <QWidget>
#include "plugin_global.h"

class FctViewForm : public IModuleForm
{
    Q_OBJECT

public:
    FctViewForm(IPlugin* plugIn, QWidget *parent = 0);
    ~FctViewForm();

    virtual bool init();
    virtual void clear();
    virtual QVector<QAction*> getActions();
};

#endif // FCTVIEWFORM_H
