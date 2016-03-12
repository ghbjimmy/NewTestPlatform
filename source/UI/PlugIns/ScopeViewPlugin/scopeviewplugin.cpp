#include "scopeviewplugin.h"
#include "scopeviewform.h"
#include "../../Util/const.h"

ScopeViewPlugin::ScopeViewPlugin()
{
    _widget = NULL;
    _name = ScopeViewPluginName;
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
    return 0;
}

bool ScopeViewPlugin::isHandleMessage(const IMessage* msg)
{
    return false;
}

QWidget * ScopeViewPlugin::createWidget()
{
    _widget = new ScopeviewForm(this);
    return _widget;
}

extern "C" IPlugin * createPlugin()
{
    return new ScopeViewPlugin();
}
