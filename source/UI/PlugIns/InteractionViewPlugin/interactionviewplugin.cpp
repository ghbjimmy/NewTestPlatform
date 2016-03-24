#include "interactionviewplugin.h"
#include "interactionviewform.h"
#include "const.h"
#include "message.h"

InteractionViewPlugin::InteractionViewPlugin()
{
    _widget = NULL;
    _name = InteractionViewPluginName;
}

int InteractionViewPlugin::init()
{
    return 0;
}

void InteractionViewPlugin::fini()
{
    if(NULL != _widget)
    {
        _widget->setParent(NULL);
    }
}

int InteractionViewPlugin::onMessage(const IMessage* msg)
{
    int ret = 0;
    if (_widget == NULL)
        return -1;

    InteractionViewForm* form = ((InteractionViewForm*)_widget);
    int id = msg->messageID();
    switch(id)
    {
    case CHANEL_STATE_MSG:
    {
        const ChannelStateMsg* statevMsg = (const ChannelStateMsg*)msg;
        form->onChanelStateMsg(statevMsg->getIndex(), statevMsg->getResult());
        break;
    }
    case PROC_ITEMSTATE_MSG:
    {
        const ProcItemStateMsg* itemMsg = (const ProcItemStateMsg*)msg;
        int evtId = itemMsg->getEventId();
        if (evtId == 0) //seq start
        {
            form->onSeqStart(itemMsg->getIndex(), itemMsg->getData());
        }
        else if (evtId == 1) //seq end
        {
            form->onSeqEnd(itemMsg->getIndex(), itemMsg->getData());
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

bool InteractionViewPlugin::isHandleMessage(const IMessage* msg)
{
    bool ret = false;
    int id = msg->messageID();
    switch(id)
    {
    case CHANEL_STATE_MSG:
    case PROC_ITEMSTATE_MSG:
    {
        ret = true;
        break;
    }
    default:
    {
        ret = false;
        break;
    }
    };
    return ret;
}

QWidget * InteractionViewPlugin::createWidget()
{
    _widget = new InteractionViewForm(this);
    return _widget;
}

extern "C" IPlugin * createPlugin()
{
    return new InteractionViewPlugin();
}
