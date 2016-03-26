#ifndef FCTVIEWFORM_H
#define FCTVIEWFORM_H

#include <QWidget>

class IPlugin;

class FctViewForm : public QWidget
{
    Q_OBJECT

public:
    FctViewForm(IPlugin* plugIn, QWidget *parent = 0);
    ~FctViewForm();

private:
    IPlugin* _plugIn;
};

#endif // FCTVIEWFORM_H
