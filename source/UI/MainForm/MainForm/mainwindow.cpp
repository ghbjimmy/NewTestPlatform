#include "mainwindow.h"

#include "libaryparser.h"
#include "pluginSubjecter.h"
#include "util.h"
#include "const.h"
#include "message.h"
#include "zmq.h"
#include "qlog.h"

#include "command.h"
#include "sequencermgr.h"
#include "zmqcfgparser.h"
#include "testenginemgr.h"
#include "statemachinemgr.h"
#include "configform.h"
#include "loadcsvform.h"
#include "loginform.h"
#include "userctrl.h"
#include "pluginsloader.h"
#include "userctrl.h"
#include "widgetprivilctrl.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QSplitter>
#include <QLibrary>
#include <QMenuBar>
#include <QFileDialog>
#include <thread>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDialog>
#include <QMessageBox>
#include <QApplication>

static void sendMessageCallBack(const IMessage* msg)
{
    MainWindow::getInstance()->dispatchMessage(msg);
}

static void LoadFileFunc(void* obj)
{
    MainWindow::getInstance()->loadFile();
}

MainWindow* MainWindow::_instance = NULL;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    _pluginSubjecter = new PluginSubjecter();

    _userCtrl = new UserCtrl();
    _wgtPrivilCtrl = new WidgetPrivilCtrl();
    setupUI();
    _instance = this;

    this->resize(1280,720);
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

bool MainWindow::init()
{
    //用户登录模块
    if (!_userCtrl->init())
    {
        LogMsg(Error, "userctrl init failed.");
        return false;
    }

    //解析模块xml
    PluginsLoader loader;
    if (!loader.parseXml())
    {
        LogMsg(Error, "parse module config failed.");
        return false;
    }

    if (!initPlugin(&loader))
    {
        LogMsg(Error, "init Plugin failed.");
        return false;
    }

    fillPluginWgt(&loader);

    _zmqCfgParse = new ZmqCfgParser();
    QString runPath = QApplication::applicationDirPath() + "/config/zmqports.json";
    if (!_zmqCfgParse->parse(runPath))
    {
        LogMsg(Error, "parse config file failed.");
        return false;
    }

    _sequencerMgr = new SequencerMgr();
    if (!_sequencerMgr->initByCfg(_zmqCfgParse))
    {
        LogMsg(Error, "init by  config file failed.");
        return false;
    }

    connect(_sequencerMgr, SIGNAL(isAliveSignal(int,bool,bool)), this, SLOT(onSeqIsAlive(int,bool,bool)));
    connect(_sequencerMgr, SIGNAL(eventSignal(int,int, const QString&)), this, SLOT(onSeqEvent(int,int, const QString&)));

    _engineMgr = new TestEngineMgr();
    if (!_engineMgr->initByCfg(_zmqCfgParse))
    {
        LogMsg(Error, "init by  config file failed.");
        return false;
    }

    connect(_engineMgr, SIGNAL(isAliveSignal(int,bool,bool)), this, SLOT(onEngIsAlive(int,bool,bool)));

    _smMgr = new StateMachineMgr();
    if (!_smMgr->initByCfg(_zmqCfgParse))
    {
        LogMsg(Error, "init by  config file failed.");
        return false;
    }

    connect(_smMgr, SIGNAL(isAliveSignal(int,bool,bool)), this, SLOT(onSmIsAlive(int,bool,bool)));

    return true;
}

bool MainWindow::initPlugin(PluginsLoader* loader)
{
    const QVector<TPluginInfo>& infos = loader->getPluginInfos();
    int size = infos.size();
    for (int i = 0; i < size; ++i)
    {
        if (infos[i].isLoad)
        {
            IPlugin* plugin = NULL;
            plugin = loadLibary(infos[i].path);
            if (NULL == plugin)
            {
                LogMsg(Error, "Load plugin libary failed.name:%s", infos[i].path.toStdString().c_str());
                return false;
            }

            _pluginSubjecter->attach(plugin);
        }
    }

    return true;
}

void MainWindow::fillPluginWgt(PluginsLoader* loader)
{
    IPlugin* plugin = NULL;
    plugin = _pluginSubjecter->getPlugin(DetailViewPluginName);
    if (NULL != plugin)
    {
        QHBoxLayout* h1 = new QHBoxLayout();
        h1->addWidget(plugin->getModuleForm());
        h1->setContentsMargins(0,0,0,0);
        _detailViewWgt->setLayout(h1);

        plugin->setStrategyID(Active);
    }

    plugin = _pluginSubjecter->getPlugin(ScopeViewPluginName);
    if (NULL != plugin)
    {
        QHBoxLayout* h1 = new QHBoxLayout();
        h1->addWidget(plugin->getModuleForm());
        h1->setContentsMargins(0,0,0,0);
        _scopeViewWgt->setLayout(h1);
        _scopeViewWgt->setMinimumHeight(0);

        plugin->setStrategyID(Active);
    }

    plugin = _pluginSubjecter->getPlugin(InteractionViewPluginName);
    if (NULL != plugin)
    {
        QHBoxLayout* h1 = new QHBoxLayout();
        h1->addWidget(plugin->getModuleForm());
        h1->setContentsMargins(0,0,0,0);
        _interViewWgt->setLayout(h1);

        plugin->setStrategyID(Active);
    }

    //填充菜单
    const QVector<TPluginInfo>& infos = loader->getPluginInfos();
    int size = infos.size();
    for (int i = 0; i < size; ++i)
    {
        if (infos[i].isExportMenu)
        {
            QMenu* curMenu = NULL;
            foreach (QMenu* menu, _menus)
            {
                if (menu->title() == infos[i].menuName)
                {
                    curMenu = menu;
                    break;
                }
            }

            if (NULL == curMenu)
                continue;

            QString pluginName = infos[i].name.section(".", 0,0);
            plugin = _pluginSubjecter->getPlugin(pluginName);
            if (plugin)
            {
                plugin->getModuleForm()->setOwner(this);
                QVector<QAction*> actions = plugin->getModuleForm()->getActions();
                for (int i = 0; i < actions.size(); ++i)
                {
                    curMenu->addAction(actions[i]);
                }
            }
        }
    }
}

void MainWindow::loadFile()
{
    //QVector<int> failVecs = _sequencerMgr->loadProfile("/Users/mac/Desktop/Hantest_plan__0322_11h.csv");

    QVector<int> failVecs = _sequencerMgr->loadProfile("/Hantest_plan__0322_11h.csv");

    if (!failVecs.empty())
    {
        LogMsg(Error, "load profile failed. failed count is %d", failVecs.size());
        //return;

        //QMessageBox::warning(this, "load file", "Load File failed. num=" + QString::number(failVecs.size()));
    }

    //list 命令
    QVector<QString> items;
    if (!_sequencerMgr->getCsvContent(items))
    {
        LogMsg(Error, "get content failed.");
        return;
    }

    if (!items.isEmpty())
    {
        ListCsvFileMsg* listCsvMsg = new ListCsvFileMsg();
        listCsvMsg->setData(items);
        //发送结果到插件
        dispatchMessage(listCsvMsg);
        delete listCsvMsg;
    }
}

void MainWindow::startLoadFile()
{
    LoadCsvForm* loadForm = new LoadCsvForm(this);
    loadForm->setProgressMaxSize(_sequencerMgr->getSeqSize());
    connect(_sequencerMgr, SIGNAL(showLoadingInfoSignal(const QString&, int)),
            loadForm, SLOT(onAppendText(const QString&, int)));
    connect(_sequencerMgr, SIGNAL(loadCompleteSignal()), loadForm, SLOT(onClose()));

    std::thread* loadFileThread = new std::thread(LoadFileFunc, this);
    loadForm->exec();

    loadFileThread->join();
    delete loadFileThread;
}

void MainWindow::startHeartBeat()
{
    _engineMgr->startAll();
    _sequencerMgr->startAll();
    _smMgr->startAll();
}

IPlugin* MainWindow::loadLibary(const QString& path)
{
    LibaryParser* parser = new LibaryParser();
    if (!parser->parse(path))
        return NULL;

    IPlugin * pPlugin = parser->getPlugin();

    pPlugin->registerSendMsgCallBack(sendMessageCallBack);
    if (0 != pPlugin->init())
    {
        LogMsg(Error, "plugin init failed.");
        delete pPlugin;
        return NULL;
    }

    _libParsers.append(parser);

    return pPlugin;
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
    _csvVersionlbl = new QLabel();
    _csvVersionlbl->setFixedHeight(10);
    _csvVersionlbl->setText("V1.0");
    QHBoxLayout* h1 = new QHBoxLayout();
    h1->addStretch(1);
    h1->addWidget(_csvVersionlbl);
    h1->addSpacerItem(new QSpacerItem(10,10, QSizePolicy::Expanding, QSizePolicy::Minimum));

    _detailViewWgt = new QWidget();
    QWidget* wgt = new QWidget();
    QVBoxLayout* v1 = new QVBoxLayout();
    v1->addLayout(h1);
    v1->addWidget(_detailViewWgt);
    wgt->setLayout(v1);
    v1->setSpacing(1);
    v1->setContentsMargins(QMargins(0,0,0,0));
    return wgt;
}

QLabel* createNumLabel(int num)
{
    QLabel* numlbl = new QLabel(QString::number(num));
    UIUtil::setBgColor(numlbl, QColor(0,255,0));

    numlbl->setAlignment(Qt::AlignCenter);
    numlbl->setFixedSize(16, 16);
    numlbl->setAutoFillBackground(true);

    return numlbl;
}

QWidget* MainWindow::createStatusWgt()
{
    QWidget* statusWgt = new QWidget();
    QHBoxLayout* h1 = new QHBoxLayout();
    QLabel* seqLbl = new QLabel("Sequencer:");
    h1->addSpacerItem(new QSpacerItem(10,10));
    h1->addWidget(seqLbl);

    for (int i = 0; i < 6; ++i)
    {
        _seqLbl[i] = createNumLabel(i);
        h1->addWidget(_seqLbl[i]);
    }

    h1->addStretch(1);
    h1->setContentsMargins(QMargins(0,0,0,0));


    QHBoxLayout* h2 = new QHBoxLayout();
    QLabel* engineLbl = new QLabel("Engine:");
    h2->addWidget(engineLbl);
    for (int i = 0; i < 6; ++i)
    {
        _engineLbl[i] = createNumLabel(i);
        h2->addWidget(_engineLbl[i]);
    }

    h2->addStretch(1);
    h2->setContentsMargins(QMargins(0,0,0,0));


    QLabel* machineLbl = new QLabel("StateMachine:");
    _smLbl = createNumLabel(0);

    QHBoxLayout* h3 = new QHBoxLayout();
    h3->addWidget(machineLbl);
    h3->addWidget(_smLbl);
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

void MainWindow::createMenu()
{
    QMenu* fileMenu = menuBar()->addMenu("File");

    _menus.push_back(fileMenu);

    QAction* confgAction = UIUtil::createWidgetWithName<QAction>("Config", "Config");
    QAction* loadAction = UIUtil::createWidgetWithName<QAction>("Load profile", "Load profile");

    _wgtPrivilCtrl->addWidget(confgAction);
    _wgtPrivilCtrl->addWidget(loadAction);

    connect(confgAction,SIGNAL(triggered()),this,SLOT(onMenuAction()));
    connect(loadAction,SIGNAL(triggered()),this,SLOT(onMenuAction()));

    fileMenu->addAction(confgAction);
    fileMenu->addAction(loadAction);

    QMenu* instruMenu = menuBar()->addMenu("Instrument");
    _menus.push_back(instruMenu);

    QMenu* userMenu = menuBar()->addMenu("User");
    QAction* loginAction = new QAction("Login",this);
    userMenu->addAction(loginAction);
    connect(loginAction,SIGNAL(triggered()),this,SLOT(onMenuAction()));

    _menus.push_back(userMenu);
}

void MainWindow::setupUI()
{
    mTitleWgt = createTitleWgt();
    mTitleWgt->setFixedHeight(80);

    QWidget* mDetailViewWgt = createDetailViewWgt();
    _scopeViewWgt = new QWidget();
    UIUtil::setBgColor(_scopeViewWgt, QColor(195,195,195));
    _scopeViewWgt->setMinimumHeight(200);

    QWidget* statWidget = createStatusWgt();
    QVBoxLayout* v11 = new QVBoxLayout();
    v11->addWidget(_scopeViewWgt, 1);
    v11->addWidget(statWidget);
    v11->setContentsMargins(0,0,0,6);

    QWidget* downWidget = new QWidget();
    downWidget->setLayout(v11);

    _interViewWgt = new QWidget();
    UIUtil::setBgColor(_interViewWgt, Qt::yellow);
    _interViewWgt->setFixedWidth(250);

    QSplitter* split = new QSplitter(Qt::Orientation::Vertical);
    split->addWidget(mDetailViewWgt);
    split->addWidget(downWidget);

    split->setStretchFactor(0, 1);
    split->setStretchFactor(1, 0);

    QHBoxLayout* h1 = new QHBoxLayout();
    h1->addWidget(split,1);
    h1->addWidget(_interViewWgt);

    QVBoxLayout* v1 = new QVBoxLayout();
    v1->addWidget(mTitleWgt);
    v1->addLayout(h1);

    v1->setContentsMargins(QMargins(3,0,3,0));
    QWidget* centralWgt = new QWidget();
    centralWgt->setLayout(v1);
    this->setCentralWidget(centralWgt);

    createMenu();
}

void MainWindow::onMenuAction()
{
    QAction* action = static_cast<QAction*>(sender());
    if (NULL == action)
        return;
    if (action->text() == "Config")
    {
        showConfigForm();
    }
    else if (action->text() == "Load profile")
    {
        showLoadCsvForm();
    }
    else if (action->text() == "Login")
    {
        showLoginForm();
    }
}


void MainWindow::showLoginForm()
{
    LoginForm* form = new LoginForm(_userCtrl);
    connect(form, SIGNAL(userPrivilSignal(const QMap<QString, int>&)), this, SLOT(onUserPrivils(const QMap<QString, int>& widgetPrivils)));
    int ret = form->exec();
    if (QDialog::Accepted == ret)
    {

    }

    delete form;
}


void MainWindow::showConfigForm()
{
    _configForm = new ConfigForm();
    _configForm->exec();
}

void MainWindow::showLoadCsvForm()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Open CSV File"), ".", tr("CSV Files(*.csv)"));
    if(path.length() == 0)
         return;

    //load 命令
    //QVector<int> failVecs = _sequencerMgr->loadProfile("/Hantest_plan_finishnormal__0322_22.csv");
    QVector<int> failVecs = _sequencerMgr->loadProfile(path);
    if (!failVecs.empty())
    {
        LogMsg(Error, "load profile failed. failed count is %d", failVecs.size());
        //return;
    }

    //list 命令
    ListCsvFileMsg* listCsvMsg = new ListCsvFileMsg();
    QVector<QString> items;
    if (!_sequencerMgr->getCsvContent(items))
    {
        LogMsg(Error, "get content failed. %d");
        delete listCsvMsg;
        return;
    }
    listCsvMsg->setData(items);
    //发送结果到插件
    dispatchMessage(listCsvMsg);
    delete listCsvMsg;
}

void MainWindow::prcoMsgBySelf(const IMessage* msg)
{
    int msgId = msg->messageID();
    if (msgId == START_TEST_MSG) //开始测试
    {
        if (!_smMgr->startTest())
        {
            LogMsg(Error, "start to test failed.");
            return;
        }
    }
    else if (msgId == STOP_TEST_MSG)
    {
        if (!_smMgr->stopTest())
        {
            LogMsg(Error, "stop to test failed.");
            return;
        }
    }
    else if (msgId == USERLOGIN_MSG)//权限控制
    {
        const UserLoginMsg* userLoginMsg = (const UserLoginMsg*)msg;
        _wgtPrivilCtrl->onWidgetPrivilCtrl(userLoginMsg->getUserPrivils());
    }
}

void MainWindow::dispatchMessage(const IMessage* msg)
{
    prcoMsgBySelf(msg);//判断是否自己需要处理的消息
    _pluginSubjecter->notity(msg); //插件处理消息
}

void showStateColor(QLabel* lbl, const QString& typeName, int index, bool isAlive, bool isShow)
{
    if (isAlive)
    {
        if (!isShow)
        {
            lbl->setText(QString::number(index));
            UIUtil::setBgColor(lbl, Qt::green);

        }
        else
        {
            lbl->setText("");
            UIUtil::setBgColor(lbl, Qt::gray);
        }

        //LogMsg(Debug, "%s %d is alvie : %d", typeName.toStdString().c_str(), index, isShow);
    }
    else
    {
        if (lbl->palette().background().color() != Qt::red)
        {
            lbl->setText(" ");
            UIUtil::setBgColor(lbl, Qt::red);

            lbl->update();
            lbl->setText(QString::number(index));
        }
    }

    lbl->update();
}

void MainWindow::onSeqIsAlive(int index, bool isAlive, bool isShow)
{
    showStateColor(_seqLbl[index], "Sequence", index, isAlive, isShow);
}

void MainWindow::onEngIsAlive(int index, bool isAlive, bool isShow)
{
    showStateColor(_engineLbl[index], "Engine", index, isAlive, isShow);
}

void MainWindow::onSmIsAlive(int index, bool isAlive, bool isShow)
{
    showStateColor(_smLbl, "StateMachine", index, isAlive, isShow);
}

void MainWindow::onSeqEvent(int index, int evt, const QString& item)
{
    //item_start item_end seq_start seq_end
    ProcItemStateMsg* procItemStarteMsg = new ProcItemStateMsg();
    procItemStarteMsg->setData(index, evt, item);

    dispatchMessage(procItemStarteMsg);
    delete procItemStarteMsg;
}

void MainWindow::onUserPrivils(const QMap<QString, int>& widgetPrivils)
{
    UserLoginMsg userLoginMsg;
    userLoginMsg.setUserPrivils(widgetPrivils);
    dispatchMessage(&userLoginMsg);
}

