#ifndef INTERACTIONVIEWFORM_H
#define INTERACTIONVIEWFORM_H

#include <QWidget>

class UutButton;
class QHBoxLayout;
class QCheckBox;

class InteractionViewForm : public QWidget
{
    Q_OBJECT

public:
    explicit InteractionViewForm(QWidget *parent = 0);
    ~InteractionViewForm();

private:
    void setupUI();
    QHBoxLayout* createBtnLayout(UutButton*& btn1, int index, const QString& text);

private:
    UutButton* _btn1;
    UutButton* _btn2;
    UutButton* _btn3;
    UutButton* _btn4;
    UutButton* _btn5;
    UutButton* _btn6;
    QCheckBox* _selBox;
};

#endif // INTERACTIONVIEWFORM_H
