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
    w.show();
    a.exec();

    return 0;
}
