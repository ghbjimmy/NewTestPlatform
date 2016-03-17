#ifndef CVSDataTreeNode_H
#define CVSDataTreeNode_H

#include <QList>
#include <QDateTime>
#include "datatreenode.h"
#include "detailviewsinterface.h"

class CVSDataTreeNode : public DataTreeNode
{

public:
    CVSDataTreeNode();
    ~CVSDataTreeNode();

    virtual void setData(void* data, int len);

public:
    inline QString getIndex() const { return _viewItem->index; }
    inline const QString& getTestKey() const { return _viewItem->testKey; }
    inline const QString& getDestcription() const { return _viewItem->destcription; }
    inline const QString& getTime() const { return _viewItem->time; }
    inline QString getLower() const { return _viewItem->lower; }
    inline QString getUpper() const { return _viewItem->upper; }
    inline const QString& getUUT1() const { return _viewItem->uut1; }
    inline const QString& getUUT2() const { return _viewItem->uut2; }
    inline const QString& getUUT3() const { return _viewItem->uut3; }
    inline const QString& getUUT4() const { return _viewItem->uut4; }
    inline const QString& getUUT5() const { return _viewItem->uut5; }
    inline const QString& getUUT6() const { return _viewItem->uut6; }

private:
    TDetailViewItem* _viewItem;
};

#endif
