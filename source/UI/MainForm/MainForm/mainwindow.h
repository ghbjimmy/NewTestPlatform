#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include "const.h"
#include "structdefine.h"

class PluginSubjecter;
class LibaryParser;
class IMessage;
class SequencerMgr;
class ZmqCfgParser;
class TestEngineMgr;
class StateMachineMgr;

class QLabel;

namespace std
{
    class thread;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool init();
    bool loadLibary(const QString& path);

    static MainWindow* getInstance();

    void dispatchMessage(const IMessage* msg);

private:
    void setupUI();
    QWidget* createDetailViewWgt();
    QWidget* createScopeViewWgt();
    QWidget* createInteractionViewWgt();

    QWidget* createStatusWgt();

    void prcoMsgBySelf(const IMessage* msg);

private slots:
    void onMenuAction();

    void onSeqIsAlive(int index, bool isAlive, bool isShow);
    void onEngIsAlive(int index, bool isAlive, bool isShow);
    void onSmIsAlive(int index, bool isAlive, bool isShow);

    void onItemStart(int index, const QString& item);
    void onItemEnd(int index, const QString& item);

private:
    static MainWindow* _instance;

    QWidget* mTitleWgt;
    PluginSubjecter* _pluginSubjecter;
    QVector<LibaryParser*> _libParsers;

    ZmqCfgParser* _zmqCfgParse;

    SequencerMgr* _sequencerMgr;    
    QLabel* _seqLbl[6];

    TestEngineMgr* _engineMgr;
    QLabel* _engineLbl[6];

    StateMachineMgr* _smMgr;
    QLabel* _smLbl;
};

#endif // MAINWINDOW_H
