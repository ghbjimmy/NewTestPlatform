#ifndef SCOPEVIEWPLUGIN_H
#define SCOPEVIEWPLUGIN_H

#include "plugin_global.h"

class TPLUGINSHARED_EXPORT ScopeViewPlugin : public IPlugin
{

public:
    ScopeViewPlugin();
    ~ScopeViewPlugin();

public:
    virtual int init();
    virtual void fini();
    virtual int onMessage(const IMessage* msg);
    virtual bool isHandleMessage(const IMessage* msg);
    virtual IModuleForm * getModuleForm();
};

#endif // SCOPEVIEWPLUGIN_H
