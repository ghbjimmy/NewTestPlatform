#ifndef DUTVIEWFORM_H
#define DUTVIEWFORM_H

#include <QWidget>
#include <QVector>

#include <QTabWidget>
#include "plugin_global.h"
class QTableWidget;
class IPlugin;
class DutViewConfigDlg;
class DutZmqRpc;
class QToolButton;
class QToolBar;
class QTextEdit;
class QPushButton;
class QComboBox;
class QLineEdit;
class QListWidget;

class DutViewForm : public IModuleForm
{
    Q_OBJECT

public:
    explicit DutViewForm(IPlugin* plugIn, QWidget *parent = 0);
    ~DutViewForm();

    virtual bool init();
    virtual void clear();
    virtual QVector<QAction*> getActions();

    bool LoadCfg(const QString& path);

private:
    void setupUI();
    QToolBar* createToolBar();
    QWidget* createSendWidget();
    QWidget* createTextEdit();

private slots:
    void onConfig();
    void onStart();
    void onStop();
    void onClear();
    void onSend();

    void onDutRecvMsg(int index, const QString& msg);

    void onAppendText(const QString& text, int state);
    void onClearCmd();

    void onShowForm();

private:
    QTabWidget* _tabWgt;

    DutViewConfigDlg* _cfgDlg;

    QVector<DutZmqRpc*> dutRpcs;

    QToolButton* _tConfigBtn;
    QToolButton* _tStartBtn;
    QToolButton* _tStopBtn;

    QToolButton* _tClearMsgBtn;

    QPushButton* _sendBtn;
    QListWidget* _cbxListWgt;
    QComboBox* _cbx;
    QVector<QTextEdit*> _msgList;

    QTextEdit* _recvDataEdit;
    QPushButton* _clearTxtBtn;

    QAction* _showFormAct;
};

#endif // DUTVIEWFORM_H
