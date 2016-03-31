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
#include "util.h"

#include <thread>
#include <QMessageBox>

TItemEnd* convertItemEnd(const QString& itemJson)
{
    QJsonParseError json_error;
    QJsonDocument document = QJsonDocument::fromJson(itemJson.toUtf8(), &json_error);
    if(json_error.error != QJsonParseError::NoError)
    {
        LogMsg(Error, "Parse ItemStart json failed. %s", itemJson.toStdString().c_str());
        return NULL;
    }

    if (!document.isObject())
    {
        LogMsg(Error, "Parse ItemStar json format is error. %s", itemJson.toStdString().c_str());
        return NULL;
    }

    TItemEnd* itemEnd = new TItemEnd();

    QJsonObject obj = document.object();
    if (obj.contains("tid"))
    {
        itemEnd->tid = obj.take("tid").toString();
    }
    if (obj.contains("value"))
    {
        itemEnd->value = obj.take("value").toString();
    }
    if (obj.contains("result"))
    {
        bool bresult1 = obj.value("result").toBool();
        int val = -9999;
        if  (-9999 == (val = obj.value("result").toInt(-9999)))
        {
            QString qstr = obj.value("result").toString("abcd");
            bool bresult = obj.value("result").isBool();
            itemEnd->result = bresult ? 1 : 0;
        }
        else
        {
            itemEnd->result = val;
        }
    }
    if (obj.contains("error"))
    {
        itemEnd->error = obj.take("error").toString();
    }
    if (obj.contains("pdca"))
    {
        itemEnd->pdca = obj.take("pdca").toString();
    }

    return itemEnd;
}

void testDecode()
{
    const char* json = "{\"jsonrpc\": \"1.0\", \"id\": \"6bb7cfe8ea4611e5b3abacbc32d422bf\", \"result\": [[0, 0, 6], [1, \"{'FUNCTION': 'station', 'PARAM1': '', 'GROUP': 'INTELLIGENT GROUNDHOG', 'DESCRIPTION': 'Get Station Type', 'VAL': '', 'HIGH': '', 'TIMEOUT': '', 'PARAM2': '{{stationName}}', 'KEY': '', 'TID': 'INTEL_HOG_100_STAT_UNITSTAGE', 'UNIT': '', 'LOW': ''}\"]]}";

    ListCmdRsp* rsp = new ListCmdRsp();
    Buffer buffer;
    buffer.setBuf(json, strlen(json));
    rsp->decode(buffer);
}
//\"-1\"
void testJson()
{
    const char* json = "{\"result\": 1,\"tid\": \"INTEL_HOG_150_STAT_COMMENT\",\"to_pdca\": true,\"value\": \"20151211.V1\"}";


    TItemEnd* end1 = convertItemEnd(json);
    int i = 0;
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

int msgId = 0x10000002;

int main(int argc, char *argv[]){

    int group = msgId >> 16;
    int id = msgId & 0xFFFF;

    int total = group | id;
    if (!total & 0x1000)
    {
        int i = 0;
    }

    if (total & 0x1001)
    {
        int i = 0;
    }

    if (total & 0x2001)
    {
        int i = 0;
    }

    if (total & 8)
    {
        int i = 0;
    }
    QString str = UIUtil::EncryptMd5("aabbcd");
    testJson();

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

    //w.startLoadFile();

    w.startHeartBeat();
    a.exec();

    return 0;
}
