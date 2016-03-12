#include "mainwindow.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QSplitter>
#include <QLibrary>
#include <QMenuBar>

#include "../../Util/libaryparser.h"
#include "../../Util/pluginSubjecter.h"
#include "../../Util/util.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    _pluginSubjecter = new PluginSubjecter();

    QString path = "D:\\Work\\tm_platform_new\\source\\build-ui-Desktop_Qt_5_5_1_MSVC2013_32bit-Debug\\PlugIns\\DetailViewPlugin\\debug\\DetailViewPlugin.dll";

    setupUI();
}

MainWindow::~MainWindow()
{
    if (_pluginSubjecter != NULL)
    {
        delete _pluginSubjecter;
        _pluginSubjecter = NULL;
    }
}

bool MainWindow::loadLibary(const QString& path)
{
    LibaryParser* parser = new LibaryParser();
    if (!parser->parse(path))
        return false;

    IPlugin * pPlugin = parser->getPlugin();
    if (0 != pPlugin->init())
        return false;

    delete parser;
    parser = NULL;

    return true;
}

QWidget* createTitleWgt()
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

    UIUtil::setBgColor(titleWgt, QColor(192,253,123));
    return titleWgt;
}

QWidget* createDetailViewWgt()
{
    QLabel* lbl = new QLabel();
    lbl->setFixedHeight(10);
    lbl->setText("V1.0");
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

QWidget* createScopeViewWgt()
{
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

QWidget* createInteractionViewWgt()
{
    LibaryParser* parser = new LibaryParser();
    QString path = "D:\\Work\\tm_platform_new\\source\\build-ui-Desktop_Qt_5_5_1_MSVC2013_32bit-Debug\\PlugIns\\InteractionViewPlugin\\debug\\InteractionViewPlugin.dll";
    if (!parser->parse(path))
    {
        return NULL;
    }

    IPlugin * pPlugin = parser->getPlugin();
    pPlugin->init();
    QWidget* viewWgt = pPlugin->createWidget();

    return viewWgt;
}

void MainWindow::setupUI()
{
    mTitleWgt = createTitleWgt();
    mTitleWgt->setFixedHeight(60);
    QWidget* mDetailViewWgt = createDetailViewWgt();
    QWidget* mScopeViewWgt = createScopeViewWgt();
    QWidget* mInteractionViewWgt = createInteractionViewWgt();
    mInteractionViewWgt->setFixedWidth(250);
    mScopeViewWgt->setMinimumHeight(200);
    QSplitter* split = new QSplitter(Qt::Orientation::Vertical);
    split->addWidget(mDetailViewWgt);
    split->addWidget(mScopeViewWgt);

    split->setStretchFactor(0, 1);

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

    QMenu* fileMenu = menuBar()->addMenu(tr("File"));
    QAction* loadAction = new QAction("Load",this);
    connect(loadAction,SIGNAL(triggered()),this,SLOT(onMenuAction()));
    fileMenu->addAction(loadAction);
    this->resize(1024,768);
}

void MainWindow::onMenuAction()
{

}
