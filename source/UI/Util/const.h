#ifndef CONST_H
#define CONST_H

#include <QString>

static const QString& InteractionViewPluginName = "InteractionViewPlugin";
static const QString& DetailViewPluginName = "DetailViewPlugin";
static const QString& ScopeViewPluginName = "ScopeViewPlugin";
static const QString& DutViewPluginName = "DutViewPlugin";
static const QString& FctViewPluginName = "FctViewPlugin";

static const int SEQ_NUM = 6;
static const int ENG_NUM = 6;
static const int SM_NUM = 1;

static const int& TIME_OUT = 3000;

//插件处理策略
enum EStrategyID
{
    InActive = 0, //非激活
    Active, //激活
    All = 2 //全部
};


#endif
