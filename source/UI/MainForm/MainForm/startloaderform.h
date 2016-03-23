#ifndef STARTLOADERFORM_H
#define STARTLOADERFORM_H

#include <QWidget>
#include <thread>

class StartLoaderForm : public QWidget
{
    Q_OBJECT

public:
    StartLoaderForm(int& isFinish, QWidget *parent = 0);
    ~StartLoaderForm();

    bool start();
    bool run();

private:
    void setupUI();

private:
    int& _isFinish;
    std::thread* _thread;
};

#endif // STARTLOADERFORM_H
