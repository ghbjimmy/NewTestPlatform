#include "mainwindow.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QSplitter>
#include <QLibrary>
#include <QMenuBar>
#include <QFileDialog>

#include "libaryparser.h"
#include "pluginSubjecter.h"
#include "util.h"
#include "const.h"
#include "message.h"
#include "zmq.h"


static void sendMessageCallBack(const IMessage* msg)
{
    MainWindow::getInstance()->dispatchMessage(msg);
}

MainWindow* MainWindow::_instance = NULL;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    _pluginSubjecter = new PluginSubjecter();

    bool flag = false;
    QString path = "D:\\Work\\tm_platform_new\\source\\build-ui-Desktop_Qt_5_5_1_MSVC2013_32bit-Debug\\PlugIns\\DetailViewPlugin\\debug\\DetailViewPlugin.dll";
    flag = loadLibary(path);

    path = "D:\\Work\\tm_platform_new\\source\\build-ui-Desktop_Qt_5_5_1_MSVC2013_32bit-Debug\\PlugIns\\ScopeViewPlugin\\debug\\ScopeViewPlugin.dll";
    flag = loadLibary(path);

    path = "D:\\Work\\tm_platform_new\\source\\build-ui-Desktop_Qt_5_5_1_MSVC2013_32bit-Debug\\PlugIns\\InteractionViewPlugin\\debug\\InteractionViewPlugin.dll";
    flag = loadLibary(path);

    setupUI();
    _instance = this;

    //int handle = zmq_close(NULL);
}

MainWindow::~MainWindow()
{
    if (_pluginSubjecter != NULL)
    {
        delete _pluginSubjecter;
        _pluginSubjecter = NULL;
    }

    foreach (LibaryParser* parser, _libParsers)
    {
        delete parser;
        parser = NULL;
    }

    _libParsers.clear();
}

MainWindow* MainWindow::getInstance()
{
    return _instance;
}

bool MainWindow::loadLibary(const QString& path)
{
    LibaryParser* parser = new LibaryParser();
    if (!parser->parse(path))
        return false;

    IPlugin * pPlugin = parser->getPlugin();
    if (0 != pPlugin->init())
    {
        delete parser;
        parser = NULL;
        return false;
    }

    pPlugin->registerSendMsgCallBack(sendMessageCallBack);
    _libParsers.append(parser);
    _pluginSubjecter->attach(pPlugin);

    return true;
}

QWidget* createTitleWgt()
{
    QLabel* nameLabel = new QLabel();
    nameLabel->setText("Calibration");
    UIUtil::setFontSize(nameLabel, 30, true);
    UIUtil::setTextColor(nameLabel, Qt::blue);
    QLabel* imgLbl = new QLabel();
    QPixmap pix= QPixmap(":/Resources/image001.png");
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

    UIUtil::setBgColor(titleWgt, Qt::gray);
    return titleWgt;
}

QWidget* MainWindow::createDetailViewWgt()
{
    QLabel* lbl = new QLabel();
    lbl->setFixedHeight(10);
    lbl->setText("V1.0");
    QHBoxLayout* h1 = new QHBoxLayout();
    h1->addStretch(1);
    h1->addWidget(lbl);
    h1->addSpacerItem(new QSpacerItem(10,10, QSizePolicy::Expanding, QSizePolicy::Minimum));

    QWidget* viewWgt = NULL;
    IPlugin * pPlugin = _pluginSubjecter->getPlugin(DetailViewPluginName);
    if (pPlugin == NULL)
        viewWgt = new QWidget();
    else
        viewWgt = pPlugin->createWidget();

    QWidget* wgt = new QWidget();
    QVBoxLayout* v1 = new QVBoxLayout();
    v1->addLayout(h1);
    v1->addWidget(viewWgt);
    wgt->setLayout(v1);
    v1->setSpacing(1);
    v1->setContentsMargins(QMargins(0,0,0,0));
    return wgt;
}

QWidget* MainWindow::createScopeViewWgt()
{
    QWidget* viewWgt = NULL;
    IPlugin * pPlugin = _pluginSubjecter->getPlugin(ScopeViewPluginName);
    if (pPlugin == NULL)
        viewWgt = new QWidget();
    else
        viewWgt = pPlugin->createWidget();

    return viewWgt;
}

QWidget* MainWindow::createInteractionViewWgt()
{
    QWidget* viewWgt = NULL;
    IPlugin * pPlugin = _pluginSubjecter->getPlugin(InteractionViewPluginName);
    if (pPlugin == NULL)
        viewWgt = new QWidget();
    else
        viewWgt = pPlugin->createWidget();

    return viewWgt;
}

QLabel* createNumLabel(int num)
{
    QLabel* numlbl = new QLabel(QString::number(num));
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(0,255,0));
    numlbl->setPalette(palette);

    numlbl->setAlignment(Qt::AlignCenter);
    numlbl->setFixedSize(16, 16);
    numlbl->setAutoFillBackground(true);

    return numlbl;
}

QWidget* createStatusWgt()
{
    QWidget* statusWgt = new QWidget();
    QHBoxLayout* h1 = new QHBoxLayout();
    QLabel* seqLbl = new QLabel("Sequencer:");
    h1->addWidget(seqLbl);

    for (int i = 0; i < 6; ++i)
    {
        h1->addWidget(createNumLabel(i));
    }

    h1->addStretch(1);
    h1->setContentsMargins(QMargins(0,0,0,0));


    QHBoxLayout* h2 = new QHBoxLayout();
    QLabel* engineLbl = new QLabel("Engine:");
    h2->addWidget(engineLbl);
    for (int i = 0; i < 6; ++i)
    {
        h2->addWidget(createNumLabel(i));
    }

    h2->addStretch(1);
    h2->setContentsMargins(QMargins(0,0,0,0));


    QLabel* machineLbl = new QLabel("StateMachine:");
    QLabel* numLbl = createNumLabel(0);

    QHBoxLayout* h3 = new QHBoxLayout();
    h3->addWidget(machineLbl);
    h3->addWidget(numLbl);
    h3->addStretch(1);
    h3->setContentsMargins(QMargins(0,0,0,0));

    QHBoxLayout* h4 = new QHBoxLayout();
    h4->addLayout(h1);
    h4->addSpacerItem(new QSpacerItem(20,10, QSizePolicy::Expanding, QSizePolicy::Expanding));
    h4->addLayout(h2);
    h4->addSpacerItem(new QSpacerItem(20,10, QSizePolicy::Expanding, QSizePolicy::Expanding));
    h4->addLayout(h3);
    h4->addStretch(1);
    h4->setContentsMargins(QMargins(0,0,0,0));

    statusWgt->setLayout(h4);

    return statusWgt;
}


void MainWindow::setupUI()
{
    mTitleWgt = createTitleWgt();
    mTitleWgt->setFixedHeight(60);
    QWidget* mDetailViewWgt = createDetailViewWgt();
    QWidget* mScopeViewWgt = createScopeViewWgt();
    QWidget* statWidget = createStatusWgt();
    QVBoxLayout* v11 = new QVBoxLayout();
    v11->addWidget(mScopeViewWgt, 1);
    v11->addWidget(statWidget);
    //v11->setContentsMargins(0,0,0,0);

    QWidget* downWidget = new QWidget();
    downWidget->setLayout(v11);

    QWidget* mInteractionViewWgt = createInteractionViewWgt();
    mInteractionViewWgt->setFixedWidth(250);

    QSplitter* split = new QSplitter(Qt::Orientation::Vertical);
    split->addWidget(mDetailViewWgt);
    split->addWidget(downWidget);

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
    QAction* loadAction = new QAction("Load CSV",this);
    QAction* loadScopeAction = new QAction("Load ScopeView",this);
    connect(loadAction,SIGNAL(triggered()),this,SLOT(onMenuAction()));
    connect(loadScopeAction,SIGNAL(triggered()),this,SLOT(onMenuAction()));
    fileMenu->addAction(loadAction);
    fileMenu->addAction(loadScopeAction);
    this->resize(1024,768);
}

void MainWindow::onMenuAction()
{
    QAction* action = static_cast<QAction*>(sender());
    if (NULL == action)
        return;
    if (action->text() == "Load CSV")
    {
        QString path = QFileDialog::getOpenFileName(this, tr("Open CSV File"), ".", tr("CSV Files(*.csv)"));
       // if(path.length() == 0)
       //     return;

        LoadCsvFileMsg* loadcsv = new LoadCsvFileMsg();
        dispatchMessage(loadcsv);
    }
    else if (action->text() == "Load ScopeView")
    {
        LoadScopeViewMsg* loadScopeViewMsg = new LoadScopeViewMsg();
        dispatchMessage(loadScopeViewMsg);
    }

}

void MainWindow::dispatchMessage(const IMessage* msg)
{
    _pluginSubjecter->notity(msg);
}
