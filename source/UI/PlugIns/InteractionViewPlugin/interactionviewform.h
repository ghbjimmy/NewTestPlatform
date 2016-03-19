#ifndef INTERACTIONVIEWFORM_H
#define INTERACTIONVIEWFORM_H

#include <QWidget>
#include <QScrollArea>

class UutButton;
class QHBoxLayout;
class QCheckBox;
class IPlugin;
class QPushButton;

class InteractionViewForm : public QScrollArea
{
    Q_OBJECT

public:
    explicit InteractionViewForm(IPlugin* plugIn, QWidget *parent = 0);
    ~InteractionViewForm();

    void onChanelStateMsg(int index, int result);

private:
    void setupUI();
    QHBoxLayout* createBtnLayout(UutButton*& btn1, int index, const QString& text);
    QHBoxLayout* createCtrlWgt();

private slots:
    void onBtnCheckBoxStatedChanged(int state);
    void onSelCheckStateChanged(int);

    void onStart();
    void onStop();

private:
    void connectBtnSignal(UutButton* btn, bool isConnect);

private:
    IPlugin* _plugIn;
    UutButton* _btn[6];
    QCheckBox* _selBox;

    QPushButton* _startBtn;
    QPushButton* _stopBtn;
};

#endif // INTERACTIONVIEWFORM_H
