#include "detailviewplugin.h"
#include "detailviewform.h"
#include "const.h"
#include "message.h"

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
    case PROC_ITEMSTATE_MSG:
    {
        const ProcItemStateMsg* itemMsg = (const ProcItemStateMsg*)msg;
        if (itemMsg->isItemStart())
        {
            form->procItemStart(itemMsg->getIndex(), itemMsg->getData());
        }
        else
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
    case PROC_ITEMSTATE_MSG:
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

QWidget * DetailViewPlugin::createWidget()
{
    _widget = new DetailViewForm(this);
    return _widget;
}

extern "C" IPlugin * createPlugin()
{
    return new DetailViewPlugin();
}
