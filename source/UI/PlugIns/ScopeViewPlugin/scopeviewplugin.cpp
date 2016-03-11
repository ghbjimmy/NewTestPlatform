#include "scopeviewplugin.h"
#include "scopeviewform.h"

ScopeViewPlugin::ScopeViewPlugin()
{
    _widget = NULL;
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
    if(NULL!=_widget)
    {
        _widget->setParent(NULL);
    }
}

int ScopeViewPlugin::onMessage(const IMessage& msg)
{
    return 0;
}

QWidget * ScopeViewPlugin::createWidget()
{
    _widget = new ScopeviewForm();
    return _widget;
}

extern "C" IPlugin * createPlugin()
{
    return new ScopeViewPlugin();
}
