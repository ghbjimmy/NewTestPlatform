#include "detailviewplugin.h"
#include "detailviewform.h"
#include "../../Util/const.h"
#include "../../Util/message.h"

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
    int id = msg->messageID();
    switch(id)
    {
        case 120:
        {
            if (_widget != NULL)
            {
                ((DetailViewForm*)_widget)->loadCsvData();
                break;
            }
        }
    default:
        return 1;
    };

    return 0;
}

bool DetailViewPlugin::isHandleMessage(const IMessage* msg)
{
    int id = msg->messageID();
    switch(id)
    {
        case 120:
        {
            return true;
        }
    };
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
