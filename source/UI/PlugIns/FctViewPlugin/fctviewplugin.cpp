#include "fctviewplugin.h"
#include "fctviewform.h"

#include <QApplication>

FctViewPlugin::FctViewPlugin()
{
    _name = "FctViewPlugin";
    _widget = new FctViewForm(this);
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

IModuleForm *FctViewPlugin::getModuleForm()
{
    return _widget;
}

extern "C" IPlugin * createPlugin()
{
    return new FctViewPlugin();
}
