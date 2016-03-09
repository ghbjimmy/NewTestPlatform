#include "detailviewplugin.h"
#include "detailviewform.h"

DetailViewPlugin::DetailViewPlugin()
{
}

int DetailViewPlugin::Init()
{
    return 0;
}

void DetailViewPlugin::Fini()
{
    if(NULL!=mWidget)
    {
        mWidget->setParent(NULL);
    }
}

// 消息解释
int DetailViewPlugin::OnMessage(const IMessage& msg)
{
    switch(msg.MessageID())
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

QWidget * DetailViewPlugin::CreateWidget()
{
    mWidget = new DetailViewForm();
    return mWidget;
}

extern "C" IPlugin * CreatePlugin()
{
    return new DetailViewPlugin();
}
