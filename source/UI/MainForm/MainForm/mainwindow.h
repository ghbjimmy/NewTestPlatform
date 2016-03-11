#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void setupUI();

private slots:
    void onMenuAction();

private:
    QWidget* mTitleWgt;
};

#endif // MAINWINDOW_H
