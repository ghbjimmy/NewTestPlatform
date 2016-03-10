#ifndef CVSDataTreeNode_H
#define CVSDataTreeNode_H

#include <QList>
#include <QDateTime>
#include "../common/datatreenode.h"

class CVSDataTreeNode : public DataTreeNode
{

public:
    CVSDataTreeNode();
    ~CVSDataTreeNode();

    virtual void setData(void* data, int len);

public:
    inline QString getIndex() const { return _index; }
    inline const QString& getTestKey() const { return _testKey; }
    inline const QString& getDestcription() const { return _destcription; }
	inline const QString& getTime() const { return _time; }
	inline QString getLower() const { return _lower; }
	inline QString getUpper() const { return _upper; }
    inline const QString& getUUT1() const { return _uut1; }
    inline const QString& getUUT2() const { return _uut2; }
    inline const QString& getUUT3() const { return _uut3; }
    inline const QString& getUUT4() const { return _uut4; }
    inline const QString& getUUT5() const { return _uut5; }
    inline const QString& getUUT6() const { return _uut6; }

private:
	QString _index;
    QString _testKey;
    QString _destcription;
	QString _time;
	QString _lower;
	QString _upper;
    QString _uut1;
    QString _uut2;
    QString _uut3;
    QString _uut4;
    QString _uut5;
    QString _uut6;
};

#endif
