#ifndef INTERACTIONVIEWFORM_H
#define INTERACTIONVIEWFORM_H

#include <QWidget>
#include <QScrollArea>

class UutButton;
class QHBoxLayout;
class QCheckBox;
class IPlugin;

class InteractionViewForm : public QScrollArea
{
    Q_OBJECT

public:
    explicit InteractionViewForm(IPlugin* plugIn, QWidget *parent = 0);
    ~InteractionViewForm();

    bool onChanelStateMsg(int index, int result);

private:
    void setupUI();
    QHBoxLayout* createBtnLayout(UutButton*& btn1, int index, const QString& text);

private slots:
    void onBtnCheckBoxStatedChanged(int state);

    void onSelCheckStateChanged(int);

private:
    IPlugin* _plugIn;
    UutButton* _btn1;
    UutButton* _btn2;
    UutButton* _btn3;
    UutButton* _btn4;
    UutButton* _btn5;
    UutButton* _btn6;
    QCheckBox* _selBox;
};

#endif // INTERACTIONVIEWFORM_H
