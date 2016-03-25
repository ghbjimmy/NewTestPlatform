#ifndef STARTLOADERFORM_H
#define STARTLOADERFORM_H

#include <QWidget>
#include <thread>

class QTextEdit;
class StartLoaderMgr;

class StartLoaderForm : public QWidget
{
    Q_OBJECT

public:
    StartLoaderForm(int& isFinish, QWidget *parent = 0);
    ~StartLoaderForm();

    bool start();
    bool run();

public slots:
    void onAppendText(const QString& text, int state);

private:
    void setupUI();

private:
    int& _isFinish;
    std::thread* _thread;
    QTextEdit* _textEdit;
    StartLoaderMgr* _startLoaderMgr;

};

#endif // STARTLOADERFORM_H
