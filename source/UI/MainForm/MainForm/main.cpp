#include "mainwindow.h"
#include <QApplication>
#include <QLibrary>
#include "../../plugins/Plugin_global.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    QLibrary * loader = new QLibrary("D:\\Work\\tm_platform_new\\source\\UI\\PlugIns\\build-DetailViewPlugin-Desktop_Qt_5_5_1_MSVC2013_32bit-Debug\\debug\\DetailViewPlugin.dll");

    loader->load();

    //QFunctionPointer * qfp = NULL;

    fnCreatePlugin fn = (fnCreatePlugin)loader->resolve("CreatePlugin");

    IPlugin * pPlugin = fn();

    // 1.
    pPlugin->Init();

    //2.
    // p->onMessage();

    //QLabel *pLabel = (QLabel *)p->CreateWidget(0);

    //QString  text = pLabel->text();

    QWidget * pwd = pPlugin->CreateWidget();


    // loader->unload();

   w.setCentralWidget(pwd);

    w.show();

    int r = a.exec();

    // w.setCentralWidget(new QLabel());

    //w.close();
    // pwd->close();
    // pwd->setParent(NULL);

    pPlugin->Fini();

    loader->unload();

    return a.exec();
}
