#include "dutviewplugin.h"
#include "dutviewform.h"
#include "message.h"
#include "const.h"

#include <QApplication>

DutViewPlugin::DutViewPlugin()
{
    _name = DutViewPluginName;
    _widget = new DutViewForm(this);
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

IModuleForm *DutViewPlugin::getModuleForm()
{
    _widget->init();
    return _widget;
}

extern "C" IPlugin * createPlugin()
{
    return new DutViewPlugin();
}
