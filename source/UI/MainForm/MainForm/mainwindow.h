#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>

class PluginSubjecter;
class LibaryParser;
class IMessage;
class SequencerMgr;
class ZmqCfgParser;

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

    bool testZmq(const char* address);

private:
    void setupUI();
    QWidget* createDetailViewWgt();
    QWidget* createScopeViewWgt();
    QWidget* createInteractionViewWgt();

private slots:
    void onMenuAction();

private:
    static MainWindow* _instance;
    QWidget* mTitleWgt;
    PluginSubjecter* _pluginSubjecter;
    QVector<LibaryParser*> _libParsers;

    SequencerMgr* _sequencerMgr;
    ZmqCfgParser* _zmqCfgParse;
};

#endif // MAINWINDOW_H
