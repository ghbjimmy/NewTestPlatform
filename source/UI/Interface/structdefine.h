#ifndef STRUCTDEFINE_H
#define STRUCTDEFINE_H

#include <QString>

struct TCsvDataItem
{
    QString group;
    QString desc;
    QString function;
    QString timeout;
    QString param1;
    QString param2;
    QString unit;
    QString low;
    QString high;
    QString key;
    QString val;
    QString tid;
};

struct TItemStart
{
    QString group;
    QString tid;
    QString unit;
    QString low;
    QString high;
    QString pdca;
};

struct TItemEnd
{
    QString tid;
    QString value;
    int result;
    QString error;
    QString pdca;
};

struct TSeqStart
{
    QString name;
    QString version;
};

struct TSeqEnd
{
    int result;
    QString error;
};

#endif // STRUCTDEFINE_H
