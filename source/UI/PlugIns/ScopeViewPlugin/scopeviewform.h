#ifndef SCOPEVIEWFORM_H
#define SCOPEVIEWFORM_H

#include <QWidget>

class ScopeviewForm : public QWidget
{
    Q_OBJECT

public:
    explicit ScopeviewForm(QWidget *parent = 0);
    ~ScopeviewForm();

private:
    void setupUI();
};

#endif // SCOPEVIEWFORM_H
