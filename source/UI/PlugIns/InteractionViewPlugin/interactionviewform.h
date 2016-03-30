#ifndef INTERACTIONVIEWFORM_H
#define INTERACTIONVIEWFORM_H

#include <QWidget>
#include <QScrollArea>
#include <QVector>

#include "plugin_global.h"

class UutButton;
class QHBoxLayout;
class QCheckBox;
class IPlugin;
class QPushButton;
class WidgetPrivilCtrl;

class InteractionViewForm : public IModuleForm
{
    Q_OBJECT

public:
    InteractionViewForm(IPlugin* plugIn, QWidget *parent = 0);
    ~InteractionViewForm();

    virtual bool init();
    virtual void clear();
    virtual QVector<QAction*> getActions();

    void onChanelStateMsg(int index, int result);

    void onSeqStart(int index, const QString& data);
    void onSeqEnd(int index, const QString& data);

    void onUserLogin(const QMap<QString, int>& userPrivils);

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
    UutButton* _btn[6];
    QCheckBox* _selBox;

    QPushButton* _startBtn;
    QPushButton* _stopBtn;

    WidgetPrivilCtrl* _wgtPrivilCtrl;
};

#endif // INTERACTIONVIEWFORM_H
