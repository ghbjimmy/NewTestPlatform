#include "interactionviewplugin.h"
#include "interactionviewform.h"

InteractionViewPlugin::InteractionViewPlugin()
{
}

int InteractionViewPlugin::init()
{
    return 0;
}

void InteractionViewPlugin::fini()
{
    if(NULL!=_widget)
    {
        _widget->setParent(NULL);
    }
}

int InteractionViewPlugin::onMessage(const IMessage& msg)
{
    return 0;
}


QWidget * InteractionViewPlugin::createWidget()
{
    _widget = new InteractionViewForm();
    return _widget;
}

extern "C" IPlugin * createPlugin()
{
    return new InteractionViewPlugin();
}
