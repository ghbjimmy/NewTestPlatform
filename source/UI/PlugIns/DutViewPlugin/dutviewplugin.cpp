#include "dutviewplugin.h"
#include "dutviewform.h"
#include "message.h"
#include <QApplication>

DutViewPlugin::DutViewPlugin()
{
    _widget = NULL;
}

DutViewPlugin::~DutViewPlugin()
{

}

int DutViewPlugin::init()
{
    return 0;
}

void DutViewPlugin::fini()
{
    if(NULL != _widget)
    {
        _widget->setParent(NULL);
    }
}

int DutViewPlugin::onMessage(const IMessage* msg)
{
    return 0;
}

bool DutViewPlugin::isHandleMessage(const IMessage* msg)
{
    return true;
}

QWidget* DutViewPlugin::createWidget()
{
    _widget = new DutViewForm(this);
    QString runPath = QApplication::applicationDirPath();
    if (!((DutViewForm*)_widget)->LoadCfg(runPath))
    {
        delete _widget;
        _widget = NULL;
        return NULL;
    }

    return _widget;
}

extern "C" IPlugin * createPlugin()
{
    return new DutViewPlugin();
}
