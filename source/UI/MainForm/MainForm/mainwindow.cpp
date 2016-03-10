#include "mainwindow.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QSplitter>
#include <QLibrary>

#include "../../plugins/Plugin_global.h"
#include "../../Util/libaryparser.h"

#include <string>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setupUI();
}

MainWindow::~MainWindow()
{

}

QWidget* CreateTitleWgt()
{

    QLabel* nameLabel = new QLabel();
    nameLabel->setText("Calibration");
    QPalette pal;
    pal.setColor(QPalette::WindowText,Qt::blue);
    nameLabel->setPalette(pal);
    QFont ft;
    ft.setPointSize(28);
    nameLabel->setFont(ft);

    QLabel* imgLbl = new QLabel();
    QPixmap pix= QPixmap("D:\\Work\\tm_platform_new\\source\\UI\\MainForm\\MainForm\\image001.png");
    imgLbl->setPixmap(pix);

    QLabel* text = new QLabel("1.29");

    QVBoxLayout* v1 = new QVBoxLayout();
    v1->addWidget(imgLbl);
    QHBoxLayout* h1 = new QHBoxLayout();
    //h1->addSpacerItem(new QSpacerItem(10,10, QSizePolicy::Minimum, QSizePolicy::Expanding));
    h1->addStretch(1);
    h1->addWidget(text);
    v1->addLayout(h1);

    QHBoxLayout* h2 = new QHBoxLayout();
    h2->addWidget(nameLabel);
    h2->addStretch(1);
    h2->addLayout(v1);
    h2->addSpacerItem(new QSpacerItem(40,10, QSizePolicy::Expanding, QSizePolicy::Expanding));
    QWidget* titleWgt = new QWidget();

    QVBoxLayout* v2 = new QVBoxLayout();
    v2->addLayout(h2);
    titleWgt->setLayout(v2);

    titleWgt->setAutoFillBackground(true);

    QPalette palette;
    palette.setColor(QPalette::Background, QColor(192,253,123));
    titleWgt->setPalette(palette);
    return titleWgt;
}

QWidget* CreateDetailViewWgt()
{
    QLabel* lbl = new QLabel();
    lbl->setFixedHeight(10);
    lbl->setText("01");
    QHBoxLayout* h1 = new QHBoxLayout();
    h1->addStretch(1);
    h1->addWidget(lbl);
    h1->addSpacerItem(new QSpacerItem(10,10, QSizePolicy::Expanding, QSizePolicy::Minimum));

    LibaryParser* parser = new LibaryParser();
    QString path = "D:\\Work\\tm_platform_new\\source\\build-ui-Desktop_Qt_5_5_1_MSVC2013_32bit-Debug\\PlugIns\\DetailViewPlugin\\debug\\DetailViewPlugin.dll";
    if (!parser->parse(path))
    {
        return NULL;
    }

    IPlugin * pPlugin = parser->getPlugin();
    pPlugin->init();
    QWidget* viewWgt = pPlugin->createWidget();

    QWidget* wgt = new QWidget();
    QVBoxLayout* v1 = new QVBoxLayout();
    v1->addLayout(h1);
    v1->addWidget(viewWgt);
    wgt->setLayout(v1);
    v1->setSpacing(1);
    v1->setContentsMargins(QMargins(0,0,0,0));
    return wgt;
}

QWidget* CreateScopeViewWgt()
{
    QWidget* wgt = new QWidget();
    wgt->setAutoFillBackground(true);

    QPalette palette;
    palette.setColor(QPalette::Background, QColor(255,0,255));
    wgt->setPalette(palette);

    LibaryParser* parser = new LibaryParser();
    QString path = "D:\\Work\\tm_platform_new\\source\\build-ui-Desktop_Qt_5_5_1_MSVC2013_32bit-Debug\\PlugIns\\ScopeViewPlugin\\debug\\ScopeViewPlugin.dll";
    if (!parser->parse(path))
    {
        return NULL;
    }

    IPlugin * pPlugin = parser->getPlugin();
    pPlugin->init();
    QWidget* viewWgt = pPlugin->createWidget();

    return viewWgt;
}

QWidget* CreateInteractionViewWgt()
{
    QWidget* wgt = new QWidget();
    wgt->setAutoFillBackground(true);

    QPalette palette;
    palette.setColor(QPalette::Background, QColor(255,255,0));
    wgt->setPalette(palette);

    return wgt;
}

void MainWindow::setupUI()
{
    mTitleWgt = CreateTitleWgt();
    mTitleWgt->setFixedHeight(60);
    QWidget* mDetailViewWgt = CreateDetailViewWgt();
    QWidget* mScopeViewWgt = CreateScopeViewWgt();
    QWidget* mInteractionViewWgt = CreateInteractionViewWgt();

    QSplitter* split = new QSplitter(Qt::Orientation::Vertical);
    split->addWidget(mDetailViewWgt);
    split->addWidget(mScopeViewWgt);

    split->setStretchFactor(0, 30);
    split->setStretchFactor(1, 1);

    QHBoxLayout* h1 = new QHBoxLayout();
    h1->addWidget(split,3);
    h1->addWidget(mInteractionViewWgt,1);

    QVBoxLayout* v1 = new QVBoxLayout();
    v1->addWidget(mTitleWgt);
    v1->addLayout(h1);

    v1->setContentsMargins(QMargins(0,0,0,0));
    QWidget* centralWgt = new QWidget();
    centralWgt->setLayout(v1);
    this->setCentralWidget(centralWgt);

    this->resize(1024,768);
}
