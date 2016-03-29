#include "scopeviewform.h"
#include "uutbutton.h"
#include "util.h"
#include "message.h"
#include "plugin_global.h"

#include <QPalette>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QGridLayout>
#include <QResizeEvent>
#include <QScrollBar>
#include <QAction>

int GWidth = 180;
int GHeight = 140;

QGroupBox* createGroupBox(int num)
{
    QGroupBox* gbox = new QGroupBox();
    gbox->setTitle("Plate" + QString::number(num));
    QGridLayout* gridLayout = new QGridLayout();

    int height = 30;
    UutButton* btn1 = new UutButton(1);
    btn1->setText("UUT1\nReady");
    btn1->setFixedHeight(height);

    UutButton* btn2 = new UutButton(1);
    btn2->setText("UUT2\nReady");
    btn2->setFixedHeight(height);

    UutButton* btn3 = new UutButton(1);
    btn3->setText("UUT3\nReady");
    btn3->setFixedHeight(height);

    UutButton* btn4 = new UutButton(1);
    btn4->setText("UUT4\nReady");
    btn4->setFixedHeight(height);

    UutButton* btn5 = new UutButton(1);
    btn5->setText("UUT5\nReady");
    btn5->setFixedHeight(height);

    UutButton* btn6 = new UutButton(1);
    btn6->setText("UUT6\nReady");
    btn6->setFixedHeight(height);

    gridLayout->addWidget(btn1,0,0,1,1);
    gridLayout->addWidget(btn2,0,1,1,1);
    gridLayout->addWidget(btn3,1,0,1,1);
    gridLayout->addWidget(btn4,1,1,1,1);
    gridLayout->addWidget(btn5,2,0,1,1);
    gridLayout->addWidget(btn6,2,1,1,1);

    gbox->setFixedSize(180,140);
    gbox->setLayout(gridLayout);

    UIUtil::setBgColor(gbox, QColor(Qt::gray));
    return gbox;
}

ScopeviewForm::ScopeviewForm(IPlugin* plugIn, QWidget *parent) : IModuleForm(plugIn, parent)
{
    setupUI();
}

ScopeviewForm::~ScopeviewForm()
{

}

bool ScopeviewForm::init()
{
    return true;
}

void ScopeviewForm::clear()
{

}

QVector<QAction*> ScopeviewForm::getActions()
{
    return QVector<QAction*>() << _showFormAct;
}

void ScopeviewForm::setupUI()
{
    _showFormAct = UIUtil::createWidgetWithName<QAction>("ScopeView Pannel");
    connect(_showFormAct, SIGNAL(triggered()), this, SLOT(onShowForm()));

    _graphView = new QGraphicsView();
    QGraphicsScene* scene = new QGraphicsScene(_graphView);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->setSceneRect(this->rect());
    _graphView->setScene(scene);
    _graphView->setCacheMode(QGraphicsView::CacheBackground);
    _graphView->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    _graphView->setBackgroundBrush(QBrush(QColor(195,195,195)));

    QVBoxLayout* vv = new QVBoxLayout();
    vv->addWidget(_graphView);
    vv->setContentsMargins(0,0,0,0);

    this->setLayout(vv);
   /* for (int i = 0; i < 8; ++i)
    {
        QGroupBox* gbox = createGroupBox(i + 1);
        _groupboxs.append(gbox);

        this->scene()->addWidget(gbox);
    }*/

    updateSceneRect();
    updateSceneItemPos();
}

void ScopeviewForm::onClicked()
{
    ChannelStateMsg msg;
    _plugin->sendMessage(&msg);
}

void ScopeviewForm::resizeEvent(QResizeEvent* evt)
{
    updateSceneRect();
    updateSceneItemPos();
    QWidget::resizeEvent(evt);
}

void ScopeviewForm::updateSceneRect()
{
    if (_groupboxs.empty())
    {
        _graphView->setSceneRect(0,0,0,GHeight + 10);
        return;
    }

    int totalItemWidth = 0;
    int totalItemHeight = 0;
    for (int i = 0; i < _groupboxs.size(); i++)
    {
        totalItemWidth += _groupboxs.at(i)->size().width();
        totalItemHeight += _groupboxs.at(i)->size().height();
    }

    int viewWidth = this->size().width();
    int viewHeight = this->size().height();

    if (viewHeight <= GHeight + 20)
    {
        _graphView->setSceneRect(0, 0, totalItemWidth, 0);
    }
    else
    {
        int num = viewWidth / GWidth;
        int sceneHeight = (_groupboxs.size() / num) * GHeight;
        _graphView->setSceneRect(0, 0, 0, sceneHeight);
    }
}

void ScopeviewForm::updateSceneItemPos()
{
    if (_groupboxs.empty())
        return;

    int yoff = 0 - _graphView->verticalScrollBar()->value();
    int xoff = 0 - _graphView->horizontalScrollBar()->value();
    int viewWidth = _graphView->size().width();
    int viewHeight = _graphView->size().height();
    int count = _groupboxs.size();

    if (viewHeight <= GHeight + 20)
    {
        for (int i = 0; i < count; ++i)
        {
            int x = xoff + i * _groupboxs.at(i)->size().width();
            int y = yoff + 5;
            QPointF point = _graphView->mapToScene(x, y);
            _groupboxs.at(i)->move(point.x() + i * 5, point.y());
        }
    }
    else
    {
        int num = viewWidth / GWidth;
        for (int i = 0; i < count; ++i)
        {
            int x = 0;
            int y = 0;
            if (i < num)
            {
                x = xoff + i * _groupboxs.at(i)->size().width() + i * 5;
                y = yoff + 5;

            }
            else
            {
                x = xoff + (i % num) * _groupboxs.at(i)->size().width() + (i % num) * 5;
                y = yoff + (i / num )* GHeight + (i / num ) * 5;
            }

            QPointF point = _graphView->mapToScene(x, y);
            _groupboxs.at(i)->move(point.x(), point.y());
        }
    }
    _graphView->update();
}

void ScopeviewForm::loadData()
{
    //暂时
    _graphView->scene()->clear();
    _groupboxs.clear();

    for (int i = 0; i < 8; ++i)
    {
        QGroupBox* gbox = createGroupBox(i + 1);
        _groupboxs.append(gbox);

        _graphView->scene()->addWidget(gbox);
    }

    updateSceneRect();
    updateSceneItemPos();
}

void ScopeviewForm::onShowForm()
{
    loadData();
}
