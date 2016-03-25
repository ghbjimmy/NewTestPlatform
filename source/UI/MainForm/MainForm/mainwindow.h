#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include "const.h"
#include <thread>

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

    inline bool isLoadFileEnd() {return _isLoadFileEnd;}
    void loadFile();

private:
    bool initPlugin();
    void fillPluginWgt();

    IPlugin*  loadLibary(const QString& path);

    void setupUI();
    QWidget* createDetailViewWgt();
    QWidget* createStatusWgt();

    void createMenu();

    void prcoMsgBySelf(const IMessage* msg);

private slots:
    void onMenuAction();

    void onSeqIsAlive(int index, bool isAlive, bool isShow);
    void onEngIsAlive(int index, bool isAlive, bool isShow);
    void onSmIsAlive(int index, bool isAlive, bool isShow);

    void onSeqEvent(int index, int evt, const QString& item);

private:
    static MainWindow* _instance;

    bool _isLoadFileEnd;
    std::thread* _loadFileThread;

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
};

#endif // MAINWINDOW_H
