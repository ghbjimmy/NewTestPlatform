#include "mainwindow.h"
#include <QApplication>

#include "qlog.h"

#include "command.h"

int main(int argc, char *argv[])
{

    LoadCsvCmdReq req;
    req.setParam("/Users/mac/Desktop/test_plan__0225_12h_optical_fct_only.csv");
    Buffer buf;
    bool ff = req.encode(buf);

    std::string str = std::string(buf.getBuf(),buf.getLen());
    QString path = "../../";
    LogMsg(Error, "open file failed.[%s]", path.toStdString().c_str());
    LogMsg(Error, "open file failed1.[%s]", path.toStdString().c_str());
    QApplication a(argc, argv);
    MainWindow w;

   // const char* ip = "tcp://172.15.3.106:5555";
    //const char* ip = "tcp://127.0.0.1:6200";
    const char* ip = "tcp://172.15.3.78:6200";
    bool flag = w.testZmq(ip);

    w.show();

    a.exec();

    return 0;
}
