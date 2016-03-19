#ifndef DETAILVIEWPLUGIN_H
#define DETAILVIEWPLUGIN_H

#include <QWidget>
#include "plugin_global.h"

class TPLUGINSHARED_EXPORT DetailViewPlugin : public IPlugin
{
public:
    DetailViewPlugin();

public:
    virtual int init();
    virtual void fini();
    virtual int onMessage(const IMessage* msg);
    virtual bool isHandleMessage(const IMessage* msg);
    virtual QWidget * createWidget();

private:
    QWidget* _widget;
};

#endif // DETAILVIEWPLUGIN_H
