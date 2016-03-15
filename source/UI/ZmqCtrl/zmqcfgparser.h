#ifndef ZMQCFGPARSER_H
#define ZMQCFGPARSER_H

#include <QString>

class ZmqCfgParser
{
public:
    ZmqCfgParser();
    ~ZmqCfgParser();
    bool parse(const QString& path);

    inline int getSeqPort() const {return _seqPort;}
    inline int getSeqPub() const {return _seqPub;}
    inline int getSmPort() const {return _smPort;}
    inline int getSmPub() const {return _smPub;}
    inline int getTestEnginePort() const {return _testEnginePort;}
    inline int getTestEnginePub() const {return _testEnginePub;}
private:
    int _seqPort;
    int _seqPub;
    int _smPort;
    int _smPub;
    int _testEnginePort;
    int _testEnginePub;
};

#endif // ZMQCFGPARSER_H
