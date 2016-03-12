#include "detailviewplugin.h"
#include "detailviewform.h"
#include "../../Util/const.h"

DetailViewPlugin::DetailViewPlugin()
{
    _widget = NULL;
    _name = DetailViewPluginName;
}

int DetailViewPlugin::init()
{
    return 0;
}

void DetailViewPlugin::fini()
{
    if(NULL != _widget)
    {
        _widget->setParent(NULL);
    }
}

// 消息解释
int DetailViewPlugin::onMessage(const IMessage* msg)
{
    switch(msg->messageID())
    {
    case PLUGIN_INIT:
    {
        //dosomethings
        break;
    }
    case PLUGIN_GUI:
    {
        //dosomethings
        break;
    }
    case PLUGIN_FINI:
    {
        //dosomethings
        break;
    }

    default:
    {
        break;
    }
    };

    return 0;
}

bool DetailViewPlugin::isHandleMessage(const IMessage* msg)
{
    return false;
}

QWidget * DetailViewPlugin::createWidget()
{
    _widget = new DetailViewForm();
    return _widget;
}

extern "C" IPlugin * createPlugin()
{
    return new DetailViewPlugin();
}
