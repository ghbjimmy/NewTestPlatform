#include "dutviewplugin.h"
#include "dutviewform.h"
#include "message.h"
#include "const.h"

#include <QApplication>

DutViewPlugin::DutViewPlugin()
{
    _name = DutViewPluginName;
    _pluginType = EPluginType::Tool;
   _widget = NULL;
}

DutViewPlugin::~DutViewPlugin()
{

}

int DutViewPlugin::init()
{
    _widget = new DutViewForm(this);
    return 0;
}

void DutViewPlugin::fini()
{
    if(NULL != _widget)
    {
        _widget->setParent(NULL);
        delete _widget;
        _widget = NULL;
    }
}

int DutViewPlugin::onMessage(const IMessage* msg)
{
    return 0;
}

bool DutViewPlugin::isHandleMessage(const IMessage* msg)
{
    //先判断是否同个组，如不是，直接返回
    if (!_pluginType & msg->groupType())
        return false;

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
