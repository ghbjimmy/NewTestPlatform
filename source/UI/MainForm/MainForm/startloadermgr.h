#ifndef STARTLOADERMGR_H
#define STARTLOADERMGR_H

#include <QObject>

class StartLoaderMgr
{
public:
    StartLoaderMgr();
    ~StartLoaderMgr();

    bool startSeq();
    bool startEng();
    bool startSM();

private:

};

#endif // STARTLOADERMGR_H
