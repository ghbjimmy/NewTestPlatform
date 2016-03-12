#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class PluginSubjecter;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    bool loadLibary(const QString& path);
private:
    void setupUI();

private slots:
    void onMenuAction();

private:
    QWidget* mTitleWgt;
    PluginSubjecter* _pluginSubjecter;
};

#endif // MAINWINDOW_H
