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

private:
    IPlugin* _plugIn;
    QTabWidget* _tabWgt;

    DutViewConfigDlg* _cfgDlg;

    QVector<DutZmqRpc*> dutRpcs;

    QToolButton* _tStartBtn;
    QToolButton* _tStopBtn;
};

#endif // DUTVIEWFORM_H
