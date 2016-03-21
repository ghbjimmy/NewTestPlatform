#include "qlog.h"
#include <QDebug>
#include <QDateTime>
#include <string>


QLog* QLog::_instance = NULL;

QLog* QLog::getInstance()
{
    if (_instance == NULL)
        _instance = new QLog();

    return _instance;
}

void QLog::destroyInstance()
{
    if (_instance != NULL)
    {
        delete _instance;
        _instance = NULL;
    }
}

QLog::QLog()
{
    m_logLevel = Debug;
}

QLog::~QLog()
{

}

std::string getNowTime()
{
    QString qstr = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    return qstr.toStdString();
}

void QLog::writeLogMsg(LogLevel level, const char* msg)
{
    if (m_logLevel > level)
        return;


    int len = strlen(msg);
    char* szLogMsg = new char[len + 256];
    sprintf(szLogMsg, "%s [%s] [%s:L%d] %s", getNowTime().c_str(), LOG_PREFIX[level],__FUNCTION__,__LINE__, msg);

    qDebug() << szLogMsg;

    delete[] szLogMsg;
    szLogMsg = NULL;
}

void LogMsg(LogLevel level, const char* format, ...)
{
    char szBuf[8096];
    va_list arglist;
    va_start(arglist, format);
    vsprintf(szBuf, format, arglist);
    va_end(arglist);

    QLog::getInstance()->writeLogMsg(level, szBuf);
}
