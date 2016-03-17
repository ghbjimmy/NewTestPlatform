#include <QSize>

#include "cvsdatatreemodel.h"
#include "cvsdatatreenode.h"

CVSDataTreeModel::CVSDataTreeModel(QObject *parent) : DataTreeModel(parent)
{

}

CVSDataTreeModel::~CVSDataTreeModel()
{

}

int CVSDataTreeModel::columnCount(const QModelIndex &parent) const
{
    return 12;
}

QVariant CVSDataTreeModel::data(const QModelIndex &index, int role) const
{
    //int row = index.row();
    int column = index.column();
    if (role == Qt::TextAlignmentRole)
    {
        if (column != 0)
        {
            return Qt::AlignCenter;
        }
    }

    if (role == Qt::SizeHintRole)
    {//这里可以指定tree每行的高度
        return QSize(60, 18);
    }
    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        CVSDataTreeNode *node = dynamic_cast<CVSDataTreeNode*>(nodeFromIndex(index));
        if (!node)
            return QVariant();

        switch (index.column())
        {
            case 0:
            {
                return node->getIndex();
            }
            case 1:
            {
                return node->getTestKey();
            }
            case 2:
            {
                return node->getDestcription();

            }
            case 3:
            {
                return node->getTime();
            }
            case 4:
            {
                return node->getLower();
            }
            case 5:
            {
                return node->getUpper();
            }
            case 6:
            {
                return node->getUUT1();
            }
            case 7:
            {
                return node->getUUT2();
            }
            case 8:
            {
                return node->getUUT3();
            }
            case 9:
            {
                return node->getUUT4();
            }
            case 10:
            {
               return node->getUUT5();
            }
            case 11:
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
	if (role != Qt::DisplayRole) 
	{

		return QVariant();
	}

	switch (section)
	{
	case 0:
		return "index";
	case 1:
		return "TestKey";
	case 2:
		return "Description";
	case 3:
		return "Time";
	case 4:
		return "Lower Limited";
	case 5:
		return "Upper Limited";
	case 6:
		return "UUT1";
	case 7:
		return "UUT2";
	case 8:
		return "UUT3";
	case 9:
		return "UUT4";
	case 10:
		return "UUT5";
	case 11:
		return "UUT6";
	}

	return QVariant();
}

