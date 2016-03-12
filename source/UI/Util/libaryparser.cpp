#include "libaryparser.h"
#include <QLibrary>

LibaryParser::LibaryParser()
{
    _loader = NULL;
}

LibaryParser::~LibaryParser()
{
    if (_loader != NULL && _loader->isLoaded())
    {
        //_loader->unload();
        delete _loader;
        _loader = NULL;
    }
}

bool LibaryParser::parse(const QString& path)
{
    _loader = new QLibrary(path);
    if (!_loader->load())
        return false;

    fnCreatePlugin fn = (fnCreatePlugin)_loader->resolve("createPlugin");
    if (fn == NULL)
        return false;

    _plugin = fn();
    if (NULL == _plugin)
        return false;

    return true;
}

IPlugin * LibaryParser::getPlugin() const
{
    return _plugin;
}
