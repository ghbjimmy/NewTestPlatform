#ifndef DETAILVIEWPLUGIN_H
#define DETAILVIEWPLUGIN_H

#include <QWidget>
#include "../Plugin_global.h"

class TPLUGINSHARED_EXPORT DetailViewPlugin : public IPlugin
{
    QWidget* mWidget;
public:
    DetailViewPlugin();

public:
    virtual int Init();

    virtual void Fini();

    virtual int OnMessage(const IMessage& msg);

    virtual QWidget * CreateWidget();

private:

};

#endif // DETAILVIEWPLUGIN_H
