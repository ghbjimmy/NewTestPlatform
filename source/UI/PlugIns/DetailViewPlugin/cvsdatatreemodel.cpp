#include <QSize>
#include <QColor>

#include "cvsdatatreemodel.h"
#include "cvsdatatreenode.h"

const int COL_INDEX = 0;
const int COL_TESTKEY = 1;
const int COL_DESC = 2;
const int COL_TIME = 3;
const int COL_UNIT = 4;
const int COL_LOWER = 5;
const int COL_UPPER = 6;
const int COL_UUT1 = 7;
const int COL_UUT2 = 8;
const int COL_UUT3 = 9;
const int COL_UUT4 = 10;
const int COL_UUT5 = 11;
const int COL_UUT6 = 12;

CVSDataTreeModel::CVSDataTreeModel(QObject *parent) : DataTreeModel(parent)
{

}

CVSDataTreeModel::~CVSDataTreeModel()
{

}

int CVSDataTreeModel::columnCount(const QModelIndex &parent) const
{
    return COL_UUT6 + 1;
}

QColor getColorByState(int state)
{//0:FAIL 1:PASS 2:SKIP -1:error 3:NORMAL
    if (state == 0)
    {
        return QColor(Qt::red);
    }
    else if (state == 1)
    {
        return QColor(Qt::blue);
    }
    else if (state == 2)
    {
        return QColor(Qt::cyan);
    }
    else if (state == -1)
    {
        return QColor(Qt::black);
    }

    else
        return Qt::black;
}

QVariant CVSDataTreeModel::data(const QModelIndex &index, int role) const
{
    CVSDataTreeNode* node = static_cast<CVSDataTreeNode*>(index.internalPointer());
    if (node == NULL)
        return QVariant();

    //int row = index.row();
    int column = index.column();
    if (role == Qt::TextAlignmentRole)
    {
        return Qt::AlignCenter;
    }

    if (role == Qt::ForegroundRole)
    {
        //0:FAIL 1:PASS 2:SKIP -1:error 3:NORMAL
        if (column == COL_UUT1)
        {
            return getColorByState(node->getUUT1State());
        }
        else if (column == COL_UUT2)
        {
            return getColorByState(node->getUUT2State());
        }
        else if (column == COL_UUT3)
        {
            return getColorByState(node->getUUT3State());
        }
        else if (column == COL_UUT4)
        {
            return getColorByState(node->getUUT4State());
        }
        else if (column == COL_UUT5)
        {
            return getColorByState(node->getUUT5State());
        }
        else if (column == COL_UUT6)
        {
            return getColorByState(node->getUUT6State());
        }

        return QColor(Qt::black);
    }

    if (role == Qt::SizeHintRole)
    {//这里可以指定tree每行的高度
        return QSize(60, 20);
    }
    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        if (!node)
            return QVariant();

        switch (index.column())
        {
        case COL_INDEX:
        {
            return node->getIndex();
        }
        case COL_TESTKEY:
        {
            return node->getTestKey();
        }
        case COL_DESC:
        {
            return node->getDestcription();

        }
        case COL_TIME:
        {
            return node->getTime();
        }
        case COL_UNIT:
        {
            return node->getUnit();
        }
        case COL_LOWER:
        {
            return node->getLower();
        }
        case COL_UPPER:
        {
            return node->getUpper();
        }
        case COL_UUT1:
        {
            return node->getUUT1();
        }
        case COL_UUT2:
        {
            return node->getUUT2();
        }
        case COL_UUT3:
        {
            return node->getUUT3();
        }
        case COL_UUT4:
        {
            return node->getUUT4();
        }
        case COL_UUT5:
        {
            return node->getUUT5();
        }
        case COL_UUT6:
        {
            return node->getUUT6();
        }
        }
    }

    return QVariant();
}

Qt::ItemFlags CVSDataTreeModel::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant CVSDataTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
	{
        switch (section)
        {
        case COL_INDEX:
            return "index";
        case COL_TESTKEY:
            return "TestKey";
        case COL_DESC:
            return "Description";
        case COL_TIME:
            return "Time";
        case COL_UNIT:
            return "Unit";
        case COL_LOWER:
            return "Lower Limited";
        case COL_UPPER:
            return "Upper Limited";
        case COL_UUT1:
            return "UUT1";
        case COL_UUT2:
            return "UUT2";
        case COL_UUT3:
            return "UUT3";
        case COL_UUT4:
            return "UUT4";
        case COL_UUT5:
            return "UUT5";
        case COL_UUT6:
            return "UUT6";
        default:
            return QVariant();
        }
	}
    else if (role == Qt::TextAlignmentRole)
    {
        return Qt::AlignCenter;
    }

	return QVariant();
}

