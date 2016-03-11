#ifndef UUTBUTTON_H
#define UUTBUTTON_H

#include <QPushButton>

class QCheckBox;
class QLabel;

class UutButton : public QPushButton
{
    Q_OBJECT

public:
    explicit UutButton(int style, QWidget *parent = 0);
    virtual ~UutButton();

    void setIndex(int index);

protected:
    virtual void resizeEvent(QResizeEvent* evt);


private:
    void setupUI();

private:
    QCheckBox* _box;
    QLabel* _txtLbl;
    int _style;
    int _index;
};

#endif // SCOPEVIEWFORM_H
