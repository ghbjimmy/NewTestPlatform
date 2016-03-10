#include "detailviewplugin.h"
#include "detailviewform.h"

DetailViewPlugin::DetailViewPlugin()
{
}

int DetailViewPlugin::init()
{
    return 0;
}

void DetailViewPlugin::fini()
{
    if(NULL!=mWidget)
    {
        mWidget->setParent(NULL);
    }
}

// 消息解释
int DetailViewPlugin::onMessage(const IMessage& msg)
{
    switch(msg.messageID())
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

QWidget * DetailViewPlugin::createWidget()
{
    mWidget = new DetailViewForm();
    return mWidget;
}

extern "C" IPlugin * createPlugin()
{
    return new DetailViewPlugin();
}
