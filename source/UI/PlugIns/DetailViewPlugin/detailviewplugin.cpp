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
        }
        case PROC_ITEMSTATE_MSG:
        {
            const ProcItemStateMsg* itemStateMsg = (const ProcItemStateMsg*)msg;
            if (itemStateMsg->isItemStart())
            {
                form->procItemStart(itemStateMsg->getIndex(), itemStateMsg->getData());
            }
            else
            {
                form->procItemEnd(itemStateMsg->getIndex(), itemStateMsg->getData());
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
        case LIST_CSV_MSG:
        case PROC_ITEMSTATE_MSG:
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
