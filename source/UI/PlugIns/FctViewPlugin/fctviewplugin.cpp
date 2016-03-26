#include "fctviewplugin.h"
#include "fctviewform.h"

#include <QApplication>

FctViewPlugin::FctViewPlugin()
{
    _widget = NULL;
}

FctViewPlugin::~FctViewPlugin()
{

}

int FctViewPlugin::init()
{
    return 0;
}

void FctViewPlugin::fini()
{
    if(NULL != _widget)
    {
        _widget->setParent(NULL);
    }
}

int FctViewPlugin::onMessage(const IMessage* msg)
{
    return 0;
}

bool FctViewPlugin::isHandleMessage(const IMessage* msg)
{
    return true;
}

QWidget* FctViewPlugin::createWidget()
{
    if (_widget != NULL)
    {
        delete _widget;
        _widget = NULL;
    }

    _widget = new FctViewForm(this);
    QString runPath = QApplication::applicationDirPath();

    return _widget;
}

extern "C" IPlugin * createPlugin()
{
    return new FctViewPlugin();
}
