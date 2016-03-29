#include "detailviewform.h"
#include <QHBoxLayout>
#include <QLabel>
#include "cvsdatatreeview.h"
#include "progressform.h"
#include "failform.h"
#include "cvsdataadapter.h"
#include "detailviewsinterface.h"
#include "detailviewplugin.h"
#include "message.h"

DetailViewForm::DetailViewForm(IPlugin* plugIn, QWidget *parent) : IModuleForm(plugIn, parent)
{
    setupUI();
}

DetailViewForm::~DetailViewForm()
{

}

bool DetailViewForm::init()
{
    return true;
}

void DetailViewForm::clear()
{

}

QVector<QAction*> DetailViewForm::getActions()
{
    return QVector<QAction*>();
}

void DetailViewForm::setupUI()
{
    _csvTreeView = new CVSDataTreeView();
    _failForm = new FailForm();
    _progressForm = new ProgressForm();

    QTabWidget* tab = new QTabWidget();
    tab->addTab(_csvTreeView , QString("Detail"));
    tab->addTab(_progressForm, QString("Progress"));
    tab->addTab(_failForm, QString("Fail Only"));

    QVBoxLayout* vv = new QVBoxLayout();
    vv->addWidget(tab);
    vv->setContentsMargins(0,0,0,0);
    this->setLayout(vv);
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

bool DetailViewForm::procItemStart(int index,  const QString& data)
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
        _csvTreeView->procItemStart(index, *itemStart, viewItem);

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
        _csvTreeView->procItemEnd(index, *itemEnd, viewItem);
        _failForm->procItemEnd(index, *itemEnd, viewItem);

        _progressForm->increaseBarValue(index);

        if (itemEnd->result == 0 || itemEnd->result == -1)//失败或着出现错误，要往外发通知其他插件结果
        {
            ChannelStateMsg msg;
             msg.setData(index, itemEnd->result);
            _plugin->sendMessage(&msg);
        }

        //结果状态标识
        if (index == 0)
        {
            viewItem->uut1State = itemEnd->result;
        }
        else if (index == 1)
        {
            viewItem->uut2State = itemEnd->result;
        }
        else if (index == 2)
        {
            viewItem->uut3State = itemEnd->result;
        }
        else if (index == 3)
        {
            viewItem->uut4State = itemEnd->result;
        }
        else if (index == 4)
        {
            viewItem->uut5State = itemEnd->result;
        }
        else if (index == 5)
        {
            viewItem->uut6State = itemEnd->result;
        }
    }

    delete itemEnd;

    return true;
}
