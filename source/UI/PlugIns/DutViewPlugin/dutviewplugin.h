#ifndef DUTVIEWPLUGIN_H
#define DUTVIEWPLUGIN_H

#include "plugin_global.h"

class TPLUGINSHARED_EXPORT DutViewPlugin : public IPlugin
{

public:
    DutViewPlugin();
    ~DutViewPlugin();
    virtual int init();
    virtual void fini();
    virtual int onMessage(const IMessage* msg);
    virtual bool isHandleMessage(const IMessage* msg);
    virtual IModuleForm * getModuleForm();
};

#endif // DUTVIEWPLUGIN_H
