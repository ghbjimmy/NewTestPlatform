#include "mainwindow.h"
#include <QApplication>
#include <QLibrary>
#include "../../plugins/Plugin_global.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    /*QLibrary * loader = new QLibrary("D:\\Work\\tm_platform_new\\source\\UI\\PlugIns\\build-DetailViewPlugin-Desktop_Qt_5_5_1_MSVC2013_32bit-Debug\\debug\\DetailViewPlugin.dll");

    loader->load();
    fnCreatePlugin fn = (fnCreatePlugin)loader->resolve("CreatePlugin");

    IPlugin * pPlugin = fn();
    pPlugin->Init();
    QWidget * pwd = pPlugin->CreateWidget();
   w.setCentralWidget(pwd);*/

    w.show();

    int r = a.exec();


    //pPlugin->Fini();

    //loader->unload();

    return 0;
}
