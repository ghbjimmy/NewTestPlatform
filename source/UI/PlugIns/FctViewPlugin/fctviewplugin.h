#ifndef FCTVIEWPLUGIN_H
#define FCTVIEWPLUGIN_H

#include <QWidget>
#include "plugin_global.h"

class TPLUGINSHARED_EXPORT FctViewPlugin : public IPlugin
{

public:
    FctViewPlugin();
    ~FctViewPlugin();

public:
    virtual int init();
    virtual void fini();
    virtual int onMessage(const IMessage* msg);
    virtual bool isHandleMessage(const IMessage* msg);
    virtual IModuleForm * getModuleForm();

};

#endif // FCTVIEWPLUGIN_H
