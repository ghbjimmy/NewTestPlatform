#ifndef QLOG_H
#define QLOG_H

enum LogLevel
{
    Debug = 0,
    Info,
    Warn,
    Error,
    Fatal
};

static const char LOG_PREFIX[5][16] = { "Debug", "Info", "Warn", "Error", "Fatal" };

//先支持往控制台输出，后续增加往文件输出或往界面输出
class QLog
{
public:
    static QLog* getInstance();
    static void destroyInstance();

    void writeLogMsg(LogLevel level, const char* msg);

private:
    QLog();
    ~QLog();
    static QLog* _instance;

private:
    LogLevel m_logLevel;
};


void LogMsg(LogLevel level, const char* format, ...);


#endif // QLOG_H
