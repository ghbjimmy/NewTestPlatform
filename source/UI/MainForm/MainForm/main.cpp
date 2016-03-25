#include "mainwindow.h"
#include <QApplication>

#include "qlog.h"

#include "command.h"
#include "ZmqSocket.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "structdefine.h"

#include <QSplashScreen>
#include <QDesktopWidget>
#include "startloaderform.h"

#include <thread>
#include <QMessageBox>
void testDecode()
{
    const char* json = "{\"jsonrpc\": \"1.0\", \"id\": \"6bb7cfe8ea4611e5b3abacbc32d422bf\", \"result\": [[0, 0, 6], [1, \"{'FUNCTION': 'station', 'PARAM1': '', 'GROUP': 'INTELLIGENT GROUNDHOG', 'DESCRIPTION': 'Get Station Type', 'VAL': '', 'HIGH': '', 'TIMEOUT': '', 'PARAM2': '{{stationName}}', 'KEY': '', 'TID': 'INTEL_HOG_100_STAT_UNITSTAGE', 'UNIT': '', 'LOW': ''}\"]]}";

    ListCmdRsp* rsp = new ListCmdRsp();
    Buffer buffer;
    buffer.setBuf(json, strlen(json));
    rsp->decode(buffer);
}

void test()
{
    // a.exec();
    //return 0;

    QPixmap pixmap("D:\\Work\\tm_platform_new\\source\\UI\\MainForm\\MainForm\\Resources\\start.jpg");
    QSplashScreen splash(pixmap);
    splash.show();


    splash.showMessage("hahahahddddddddxxxxxx",  Qt::AlignRight | Qt::AlignTop);
    splash.showMessage("111hahahahddddddddxxxxxx", Qt::AlignRight | Qt::AlignTop);
    splash.showMessage("22222hahahahddddddddxxxxxx", Qt::AlignRight | Qt::AlignTop);
    splash.showMessage("3333hahahahddddddddxxxxxx", Qt::AlignRight | Qt::AlignTop);
    splash.showMessage("4444hahahahddddddddxxxxxx", Qt::AlignRight | Qt::AlignTop);


    void *context = zmq_ctx_new ();
    void *zsocket = zmq_socket(context, ZMQ_REQ);

    int ret1 = zmq_connect(zsocket, "tcp://127.0.0.1:5560");

    zmq_setsockopt(zsocket, ZMQ_RCVTIMEO, (void*)&TIME_OUT, sizeof(int));

    char buffer [255];
    char *send_s = "Hello";
    int len11 = zmq_send (zsocket, send_s, strlen(send_s), 0);

    char szBuf1[1024];
    int len4 = zmq_recv(zsocket, &szBuf1, sizeof(szBuf1), 0);

    zmq_close (zsocket);
    zmq_ctx_shutdown (context);

    ZmqSocket* reqSocket = new ZmqSocket(ZMQ_REQ);
    if (!reqSocket->connect("127.0.0.1", 9950))
    {
        //LogMsg(Error, "connet sequencer req failed. ip:%s port %d", reqIp.toStdString().c_str(), reqPort);
        return;
    }

    reqSocket->setSockOpt(ZMQ_RCVTIMEO, (void*)&TIME_OUT, sizeof(int));
    reqSocket->setSockOpt(ZMQ_SNDTIMEO, (void*)&TIME_OUT, sizeof(int));
    reqSocket->sendData("fad", 3);

    Buffer rbuf;
    int ret = reqSocket->recvData(rbuf);
    delete reqSocket;


    LogMsg(Error, "fadaf");
    TItemStart itemStart;
    int ss1 = sizeof(TItemStart);

    itemStart.group = "1234";
    itemStart.tid = "abdef";
    int ss2 = sizeof(TItemStart);
    //testDecode();
    // return 1;
    const char* sbuf = "{\"jsonrpc\": \"1.0\", \"id\": \"{3ecdd30a-8040-4084-b582-25dbac813e3a}\", \"result\": \"/Users/mac/Desktop/test_plan__0225_12h_optical_fct_only.csv has been loaded\"}";
    int len = strlen(sbuf);
    char* szBuf = new char[len + 1];
    memcpy(szBuf, sbuf, len);
    szBuf[len] = '\0';

    QString qstr = "{\"jsonrpc\": \"1.0\", \"id\": \"{3ecdd30a-8040-4084-b582-25dbac813e3a}\", \"result\": \"/Users/mac/Desktop/test_plan__0225_12h_optical_fct_only.csv has been loaded\"}";
    //QString qstr = QString::fromStdString(szBuf);
    QByteArray array(szBuf, len);
    QJsonParseError json_error;
    QJsonDocument document = QJsonDocument::fromJson(array, &json_error);
    if (!document.isObject())
    {
        delete szBuf;
        return;
    }

    LoadCsvCmdReq req;
    req.setParam("/Users/mac/Desktop/Hantest_plan__0322_11h.csv");
    Buffer buf;
    bool ff = req.encode(buf);
}

int main(int argc, char *argv[]){

    QApplication a(argc, argv);


    int isFinish = 0;

    StartLoaderForm startLoadForm(isFinish);
    startLoadForm.show();
    startLoadForm.move ((QApplication::desktop()->width() - startLoadForm.width())/2,
                        (QApplication::desktop()->height() - startLoadForm.height())/2);


    if (!startLoadForm.start())
    {
        QMessageBox::critical(NULL, "Start progress.","Start progress failed.");
        return -1;
    }

    while(isFinish == 0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        a.processEvents();
    }

    startLoadForm.close();




    MainWindow w;

    if (!w.init())
    {
        QMessageBox::critical(NULL, "Init.","Main Form Init Failed.");
        return -1;
    }
    w.show();
    a.processEvents();
    a.processEvents();

    w.startLoadFile();

    w.startHeartBeat();
    a.exec();

    return 0;
}
