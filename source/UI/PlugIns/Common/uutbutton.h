#ifndef UUTBUTTON_H
#define UUTBUTTON_H

#include <QPushButton>

class QCheckBox;
class QLabel;

class UutButton : public QPushButton
{
    Q_OBJECT

public:
    explicit UutButton(QWidget *parent = 0);
    virtual ~UutButton();

private:
    void setupUI();

private:
    QCheckBox* _box;
    QLabel* _txtLbl;
};

#endif // SCOPEVIEWFORM_H
