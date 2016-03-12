#ifndef SCOPEVIEWFORM_H
#define SCOPEVIEWFORM_H

#include <QScrollArea>

class IPlugin;
class ScopeviewForm : public QWidget
{
    Q_OBJECT

public:
    explicit ScopeviewForm(IPlugin* plugIn, QWidget *parent = 0);
    ~ScopeviewForm();

private:
    void setupUI();

private slots:
    void onBtnCheckBoxStatedChanged(int state);

private:
    IPlugin* _plugIn;
};

#endif // SCOPEVIEWFORM_H
