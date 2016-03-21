#ifndef ZMQCFGPARSER_H
#define ZMQCFGPARSER_H

#include <QString>

class ZmqCfgParser
{
public:
    ZmqCfgParser();
    ~ZmqCfgParser();
    bool parse(const QString& path);

    inline const QString& getSeqIp() const {return _seqIp;}
    inline const QString& getTestEngineIp() const {return _engineIp;}
    inline const QString& getSmIp() const {return _smIp;}

    inline int getSeqPort() const {return _seqPort;}
    inline int getSeqPub() const {return _seqPub;}
    inline int getSmPort() const {return _smPort;}
    inline int getSmPub() const {return _smPub;}
    inline int getSmHeartBeat() const {return _smHeartBeat;}
    inline int getTestEnginePort() const {return _testEnginePort;}
    inline int getTestEnginePub() const {return _testEnginePub;}

private:
    QString _seqIp;
    QString _engineIp;
    QString _smIp;

    int _seqPort;
    int _seqPub;
    int _smPort;
    int _smPub;
    int _smHeartBeat;
    int _testEnginePort;
    int _testEnginePub;
};

#endif // ZMQCFGPARSER_H
