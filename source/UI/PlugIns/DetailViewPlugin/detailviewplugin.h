#ifndef DETAILVIEWPLUGIN_H
#define DETAILVIEWPLUGIN_H

#include <QWidget>
#include "plugin_global.h"

class TPLUGINSHARED_EXPORT DetailViewPlugin : public IPlugin
{
public:
    DetailViewPlugin();
    ~DetailViewPlugin();

public:
    virtual int init();
    virtual void fini();
    virtual int onMessage(const IMessage* msg);
    virtual bool isHandleMessage(const IMessage* msg);
    virtual IModuleForm * getModuleForm();

};

#endif // DETAILVIEWPLUGIN_H
