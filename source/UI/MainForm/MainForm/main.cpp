#include "mainwindow.h"
#include <QApplication>

#include "qlog.h"

#include "command.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "structdefine.h"


void testDecode()
{
    const char* json = "{\"jsonrpc\": \"1.0\", \"id\": \"6bb7cfe8ea4611e5b3abacbc32d422bf\", \"result\": [[0, 0, 6], [1, \"{'FUNCTION': 'station', 'PARAM1': '', 'GROUP': 'INTELLIGENT GROUNDHOG', 'DESCRIPTION': 'Get Station Type', 'VAL': '', 'HIGH': '', 'TIMEOUT': '', 'PARAM2': '{{stationName}}', 'KEY': '', 'TID': 'INTEL_HOG_100_STAT_UNITSTAGE', 'UNIT': '', 'LOW': ''}\"]]}";

    ListCmdRsp* rsp = new ListCmdRsp();
    Buffer buffer;
    buffer.setBuf(json, strlen(json));
    rsp->decode(buffer);
}


int main(int argc, char *argv[])
{
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
        return false;
    }


    LoadCsvCmdReq req;
    req.setParam("/Users/mac/Desktop/test_plan__0225_12h_optical_fct_only.csv");
    Buffer buf;
    bool ff = req.encode(buf);


    QApplication a(argc, argv);
    MainWindow w;

   // const char* ip = "tcp://172.15.3.106:5555";
    //const char* ip = "tcp://127.0.0.1:6200";
    //const char* ip = "tcp://172.15.3.78:6200";
    //bool flag = w.testZmq(ip);
    w.init();
    w.show();

    a.exec();

    return 0;
}
