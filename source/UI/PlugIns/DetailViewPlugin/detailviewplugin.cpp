#include "detailviewplugin.h"
#include "detailviewform.h"
#include "const.h"
#include "message.h"

DetailViewPlugin::DetailViewPlugin()
{
    _name = DetailViewPluginName;
    _widget = new DetailViewForm(this);
}

DetailViewPlugin::~DetailViewPlugin()
{

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
    int ret = 0;
    if (_widget == NULL)
        return -1;

    DetailViewForm* form = ((DetailViewForm*)_widget);
    int id = msg->messageID();
    switch(id)
    {
    case LIST_CSV_MSG:
    {
        const ListCsvFileMsg* listcsvMsg = (const ListCsvFileMsg*)msg;
        form->listCsvData(listcsvMsg->getItems());
        break;
    }
    case PROC_PUBEVENT_MSG:
    {
        const ProcItemStateMsg* itemMsg = (const ProcItemStateMsg*)msg;
        int evtId = itemMsg->getEventId();
        if (evtId == 2) //item start
        {
            form->procItemStart(itemMsg->getIndex(), itemMsg->getData());
        }
        else if (evtId == 3) //item end
        {
            form->procItemEnd(itemMsg->getIndex(), itemMsg->getData());
        }

        break;
    }
    default:
    {
        ret = -1;
        break;
    }
    };

    return ret;
}

bool DetailViewPlugin::isHandleMessage(const IMessage* msg)
{
    bool ret = false;
    int id = msg->messageID();
    switch(id)
    {
    case LIST_CSV_MSG:
    case PROC_PUBEVENT_MSG:
    {
        ret = true;
        break;
    }
    default:
        ret = false;
        break;
    };
    return ret;
}

IModuleForm *DetailViewPlugin::getModuleForm()
{
    return _widget;
}

extern "C" IPlugin * createPlugin()
{
    return new DetailViewPlugin();
}
