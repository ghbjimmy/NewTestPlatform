#ifndef INTERACTIONVIEWPLUGIN_H
#define INTERACTIONVIEWPLUGIN_H

#include "plugin_global.h"

class TPLUGINSHARED_EXPORT InteractionViewPlugin : public IPlugin
{

public:
    InteractionViewPlugin();

public:
    virtual int init();
    virtual void fini();
    virtual int onMessage(const IMessage* msg);
    virtual bool isHandleMessage(const IMessage* msg);
    virtual IModuleForm * getModuleForm();

};

#endif // INTERACTIONVIEWPLUGIN_H
