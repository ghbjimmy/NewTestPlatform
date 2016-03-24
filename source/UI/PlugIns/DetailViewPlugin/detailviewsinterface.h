#ifndef DETAILVIEWPINTERFACE_H
#define DETAILVIEWPINTERFACE_H

#include <QString>

struct TDetailViewItem
{
    QString index;
    QString group;
    QString testKey;
    QString destcription;
	QString time;
	QString unit;
	QString lower;
	QString upper;
    QString uut1;
    QString uut2;
    QString uut3;
    QString uut4;
    QString uut5;
    QString uut6;

    //0:FAIL 1:PASS 2:SKIP -1:error 3:NORMAL
    int uut1State;
    int uut2State;
    int uut3State;
    int uut4State;
    int uut5State;
    int uut6State;

    TDetailViewItem()
    {
        uut1State = 3;
        uut2State = 3;
        uut3State = 3;
        uut4State = 3;
        uut5State = 3;
        uut6State = 3;
    }
};

#endif
