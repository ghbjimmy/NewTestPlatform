#include "mainwindow.h"
#include <QApplication>

#include "qlog.h"

int main(int argc, char *argv[])
{
    QString path = "../../";
    LogMsg(Error, "open file failed.[%s]", path.toStdString().c_str());
    LogMsg(Error, "open file failed1.[%s]", path.toStdString().c_str());
    QApplication a(argc, argv);
    MainWindow w;

   // const char* ip = "tcp://172.15.3.106:5555";
    const char* ip = "tcp://127.0.0.1:6200";
    bool flag = w.testZmq(ip);

    w.show();

    a.exec();

    return 0;
}
