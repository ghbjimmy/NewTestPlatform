#include "scopeviewplugin.h"
#include "scopeviewform.h"
#include "const.h"
#include "message.h"

ScopeViewPlugin::ScopeViewPlugin()
{
    _name = ScopeViewPluginName;
    _widget = new ScopeviewForm(this);
}

ScopeViewPlugin::~ScopeViewPlugin()
{

}

int ScopeViewPlugin::init()
{
    return 0;
}

void ScopeViewPlugin::fini()
{
    if(NULL != _widget)
    {
        _widget->setParent(NULL);
    }
}

int ScopeViewPlugin::onMessage(const IMessage* msg)
{
    int ret = 0;
    int id = msg->messageID();
    switch(id)
    {
    case 130:
    {
        if (_widget != NULL)
        {
            ((ScopeviewForm*)_widget)->loadData();
            break;
        }
    }
    default:
        ret = -1;
        break;
    };

    return ret;
}

bool ScopeViewPlugin::isHandleMessage(const IMessage* msg)
{
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
