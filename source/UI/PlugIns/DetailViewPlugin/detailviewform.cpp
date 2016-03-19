#include "detailviewform.h"
#include <QHBoxLayout>
#include <QLabel>
#include "cvsdatatreeview.h"
#include "progressform.h"
#include "failform.h"
#include "cvsdataadapter.h"
#include "detailviewsinterface.h"

DetailViewForm::DetailViewForm(QWidget *parent) : QTabWidget(parent)
{
    CVSDataTreeView* view = new CVSDataTreeView();
    setupUI();
}

DetailViewForm::~DetailViewForm()
{

}

void DetailViewForm::setupUI()
{
    _csvTreeView = new CVSDataTreeView();
    _failForm = new FailForm();

    _progressForm = new ProgressForm();
    this->addTab(_csvTreeView , QString("Detail"));
    this->addTab(_progressForm, QString("Progress"));
    this->addTab(_failForm, QString("Fail Only"));
}

bool DetailViewForm::listCsvData(const QVector<QString>& datas)
{
    CVSDataTreeNode* rootNode = CVSDataAdapter::convertData(datas, _vieItems);
    if (NULL == rootNode)
        return false;
    _csvTreeView->setRootNode(rootNode);
    _progressForm->setBarMaxSize(_vieItems.size());
    return true;
}

bool DetailViewForm::procItemStart(int index, const QString& data)
{
    TItemStart* itemStart = CVSDataAdapter::convertItemStart(data);
    if (NULL == itemStart)
        return false;

    TDetailViewItem* viewItem = NULL;
    for (int i = 0; i < _vieItems.size(); ++i)
    {
        if (itemStart->group == _vieItems[i]->group && itemStart->tid == _vieItems[i]->testKey)
        {
            viewItem = _vieItems[i];
            break;
        }
    }

    if (viewItem != NULL)
        _csvTreeView->procItemStart(index, itemStart, viewItem);

    delete itemStart;
    return true;
}

bool DetailViewForm::procItemEnd(int index, const QString& data)
{
    TItemEnd* itemEnd = CVSDataAdapter::convertItemEnd(data);
    if (NULL == itemEnd)
        return false;

    TDetailViewItem* viewItem = NULL;
    for (int i = 0; i < _vieItems.size(); ++i)
    {
        if (itemEnd->tid == _vieItems[i]->testKey)
        {
            viewItem = _vieItems[i];
            break;
        }
    }

    if (NULL != viewItem)
    {
        _csvTreeView->procItemEnd(index, itemEnd, viewItem);
        _failForm->procItemEnd(index, itemEnd, viewItem);

        _progressForm->increaseBarValue(index);
    }

    delete itemEnd;
    return true;
}
