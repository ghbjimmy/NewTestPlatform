#include "scopeviewform.h"
#include <QPalette>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QGridLayout>
#include <QResizeEvent>
#include <QScrollBar>
#include "uutbutton.h"
#include "util.h"
#include "message.h"
#include "plugin_global.h"

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

ScopeviewForm::ScopeviewForm(IPlugin* plugIn, QWidget *parent) : QGraphicsView(parent)
{
    _plugIn = plugIn;
    setupUI();
}

ScopeviewForm::~ScopeviewForm()
{

}

void ScopeviewForm::setupUI()
{
    QGraphicsScene* scene = new QGraphicsScene(this);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->setSceneRect(this->rect());
    setScene(scene);
    setCacheMode(QGraphicsView::CacheBackground);
    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    this->setBackgroundBrush(QBrush(Qt::gray));

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
    _plugIn->sendMessage(&msg);
}

void ScopeviewForm::resizeEvent(QResizeEvent* evt)
{
    updateSceneRect();
    updateSceneItemPos();
    QGraphicsView::resizeEvent(evt);
}

void ScopeviewForm::updateSceneRect()
{
    if (_groupboxs.empty())
    {
        this->setSceneRect(0,0,0,GHeight + 10);
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
        this->setSceneRect(0, 0, totalItemWidth, 0);
    }
    else
    {
        int num = viewWidth / GWidth;
        int sceneHeight = (_groupboxs.size() / num) * GHeight;
        this->setSceneRect(0, 0, 0, sceneHeight);
    }
}

void ScopeviewForm::updateSceneItemPos()
{
    if (_groupboxs.empty())
        return;

    int yoff = 0 - this->verticalScrollBar()->value();
    int xoff = 0 - this->horizontalScrollBar()->value();
    int viewWidth = this->size().width();
    int viewHeight = this->size().height();
    int count = _groupboxs.size();

    if (viewHeight <= GHeight + 20)
    {
        for (int i = 0; i < count; ++i)
        {
            int x = xoff + i * _groupboxs.at(i)->size().width();
            int y = yoff + 5;
            QPointF point = mapToScene(x, y);
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

            QPointF point = mapToScene(x, y);
            _groupboxs.at(i)->move(point.x(), point.y());
        }
    }
    this->update();
}

void ScopeviewForm::loadData()
{
    for (int i = 0; i < 8; ++i)
    {
        QGroupBox* gbox = createGroupBox(i + 1);
        _groupboxs.append(gbox);

        this->scene()->addWidget(gbox);
    }

    updateSceneRect();
    updateSceneItemPos();
}
