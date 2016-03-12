#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>

class PluginSubjecter;
class LibaryParser;
class IMessage;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool loadLibary(const QString& path);

    static MainWindow* getInstance();

    void dispatchMessage(const IMessage* msg);

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
};

#endif // MAINWINDOW_H
