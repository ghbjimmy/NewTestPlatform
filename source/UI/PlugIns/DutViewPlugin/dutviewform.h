#ifndef DUTVIEWFORM_H
#define DUTVIEWFORM_H

#include <QWidget>
#include <QVector>

#include <QTabWidget>

class QTableWidget;
class IPlugin;
class DutViewConfigDlg;
class DutZmqRpc;
class QToolButton;
class QToolBar;
class QListWidget;
class QPushButton;
class QComboBox;
class QLineEdit;

class DutViewForm : public QWidget
{
    Q_OBJECT

public:
    explicit DutViewForm(IPlugin* plugIn, QWidget *parent = 0);
    ~DutViewForm();

    bool LoadCfg(const QString& path);

private:
    void setupUI();
    QToolBar* createToolBar();

private slots:
    void onConfig();
    void onStart();
    void onStop();
    void onClear();
    void onScroll();
    void onSend();

    void onDutRecvMsg(int index, const QString& msg);

private:
    IPlugin* _plugIn;
    QTabWidget* _tabWgt;

    DutViewConfigDlg* _cfgDlg;

    QVector<DutZmqRpc*> dutRpcs;

    QToolButton* _tConfigBtn;
    QToolButton* _tStartBtn;
    QToolButton* _tStopBtn;

    QToolButton* _tClearMsgBtn;
    QToolButton* _tSelAutoScrollBtn;

    QPushButton* _sendBtn;
    QListWidget* _cbxListWgt;
    QComboBox* _cbx;
    QVector<QListWidget*> _msgList;

    QLineEdit* _recvDataEdit;
    bool _isAutoScroll;
};

#endif // DUTVIEWFORM_H
