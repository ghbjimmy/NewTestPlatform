#include "scopeviewplugin.h"
#include "scopeviewform.h"
#include "const.h"
#include "message.h"

ScopeViewPlugin::ScopeViewPlugin()
{
    _name = ScopeViewPluginName;
    _pluginType = EPluginType::Device;
    _widget = NULL;
}

ScopeViewPlugin::~ScopeViewPlugin()
{

}

int ScopeViewPlugin::init()
{
    _widget = new ScopeviewForm(this);
    return 0;
}

void ScopeViewPlugin::fini()
{
    if(NULL != _widget)
    {
        _widget->setParent(NULL);
        delete _widget;
        _widget = NULL;
    }
}

int ScopeViewPlugin::onMessage(const IMessage* msg)
{
    int ret = 0;
    int id = msg->messageID();
    switch(id)
    {
    default:
        ret = -1;
        break;
    };

    return ret;
}

bool ScopeViewPlugin::isHandleMessage(const IMessage* msg)
{
    //先判断是否同个组，如不是，直接返回
    if (!_pluginType & msg->groupType())
        return false;

    bool ret =false;
    int id = msg->messageID();
    switch(id)
    {
    case 130:
    {
        ret = true;
        break;
    }
    default:
    {
        ret = false;
        break;
    }
    };
    return ret;
}

IModuleForm *ScopeViewPlugin::getModuleForm()
{
    return _widget;
}

extern "C" IPlugin * createPlugin()
{
    return new ScopeViewPlugin();
}
