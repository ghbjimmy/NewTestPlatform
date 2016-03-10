#ifndef LIBARYPARSER_H
#define LIBARYPARSER_H

#include "../plugins/Plugin_global.h"

class QLibrary;

class LibaryParser
{
public:
    LibaryParser();
    ~LibaryParser();

    bool parse(const QString& path);

    IPlugin* getPlugin() const;

private:
    QLibrary * _loader;
    IPlugin* _plugin;
};

#endif
