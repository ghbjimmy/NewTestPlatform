#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QMap>
#include "const.h"

class IPlugin;
class PluginSubjecter;
class LibaryParser;
class IMessage;
class SequencerMgr;
class ZmqCfgParser;
class TestEngineMgr;
class StateMachineMgr;

class QLabel;
class ConfigForm;
class PluginsLoader;
class UserCtrl;
class WidgetPrivilCtrl;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool init();

    void startLoadFile();
    void startHeartBeat();

    static MainWindow* getInstance();
    void dispatchMessage(const IMessage* msg);

    void loadFile();

private:
    bool initPlugin(PluginsLoader* loader);
    void fillPluginWgt(PluginsLoader* loader);

    IPlugin*  loadLibary(const QString& path);

    void setupUI();
    QWidget* createDetailViewWgt();
    QWidget* createStatusWgt();

    void createMenu();

    void prcoMsgBySelf(const IMessage* msg);

    void showConfigForm();
    void showLoadCsvForm();
    void showLoginForm();

private slots:
    void onMenuAction();

    void onSeqIsAlive(int index, bool isAlive, bool isShow);
    void onEngIsAlive(int index, bool isAlive, bool isShow);
    void onSmIsAlive(int index, bool isAlive, bool isShow);

    void onSeqEvent(int index, int evt, const QString& item);

    //响应用户登录
    void onUserPrivils(const QMap<QString, int>& widgetPrivils);



private:
    static MainWindow* _instance;

    QWidget* mTitleWgt;
    QWidget* _detailViewWgt;
    QWidget* _scopeViewWgt;
    QWidget* _interViewWgt;
    QLabel* _csvVersionlbl;

    PluginSubjecter* _pluginSubjecter;
    QVector<LibaryParser*> _libParsers;

    ZmqCfgParser* _zmqCfgParse;

    SequencerMgr* _sequencerMgr;    
    QLabel* _seqLbl[6];

    TestEngineMgr* _engineMgr;
    QLabel* _engineLbl[6];

    StateMachineMgr* _smMgr;
    QLabel* _smLbl;

    ConfigForm* _configForm;

    QVector<QMenu*> _menus;

    UserCtrl* _userCtrl;
    WidgetPrivilCtrl* _wgtPrivilCtrl;
};

#endif // MAINWINDOW_H
