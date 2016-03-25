#ifndef STARTLOADERMGR_H
#define STARTLOADERMGR_H

#include <QObject>

class StartLoaderMgr : public QObject
{
    Q_OBJECT

public:
    StartLoaderMgr();
    ~StartLoaderMgr();

    bool init();
    bool startSeq();
    bool startEng();
    bool startSM();

signals:
    void showStartingInfoSignal(const QString& text, int state);

private:

};

#endif // STARTLOADERMGR_H
