/*
 * =====================================================================================
 *
 *       Filename:  PluginInfo.h
 *
 *    Description:  插件内部对应的功能
 *
 *        Version:  1.0
 *        Created:  2021年08月07日 21时15分41秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#pragma once

#include "BaseCode.h"
#include "MyDB.h"

struct PluginEntryItem
{
    uint32_t        msgId;
    CMyString       cgiName;
    CMyString       funcName;

    PluginEntryItem(uint32_t _msgId, const CMyString& _func)
        : msgId(_msgId), cgiName(), funcName(_func)
    {
    }
    PluginEntryItem(const CMyString &_cgi, const CMyString& _func)
        : msgId(0), cgiName(_cgi), funcName(_func)
    {
    }
};
typedef std::vector<PluginEntryItem>    VEC_ENTRY;

// 插件中注册插件函数的宏定义
#define REG_CGI(vec, cgi, func)     vec.push_back(PluginEntryItem((cgi), (func)))
#define REG_MSG(vec, msg, func)     vec.push_back(PluginEntryItem((msg), (func)))

struct PluginFunc
{
    CMyString       strUpdate;          // 更新插件
    CMyString       strGetEntry;        // 获得插件内函数
    CMyString       strMaintance;       // 插件定时操作
    CMyString       strFinial;          // 释放插件
};

/* 初始化函数,传入公共的数据库指针
 * 参数:  [IN]  const CMyString&        配置的xml文件
 *        [IN]  const CMyString&        xml文件中对应的节
 *        [OUT] struct PluginFunc&      插件中配置好函数名称
 * 返回值：0   装载并初始化成功
 *         <0  初始化失败
 */
typedef int32_t (*PFN_PLUGIN_INIT)(const CMyString&, const CMyString&, struct PluginFunc&);
// 插件内部数据更新
typedef void (*PFN_PLUGIN_UPDATE)(void);
// 获得插件内的功能
typedef void (*PFN_PLUGIN_GETENTRY)(VEC_ENTRY&);
// 插件定时循环操作
typedef void (*PFN_PLUGIN_MAINTANCE)(void);
// 插件卸载
typedef void (*PFN_PLUGIN_FINI)(void);

struct PluginFuncPtr
{
    PFN_PLUGIN_INIT         pfnInit;        // Init
    PFN_PLUGIN_UPDATE       pfnUpdate;      // Update
    PFN_PLUGIN_GETENTRY     pfnGetEntry;    // GetEntry
    PFN_PLUGIN_MAINTANCE    pfnMaintance;   // Maintance
    PFN_PLUGIN_FINI         pfnFinial;      // Finial

    PluginFuncPtr()
        : pfnInit(NULL)
        , pfnUpdate(NULL)
        , pfnGetEntry(NULL)
        , pfnMaintance(NULL)
        , pfnFinial(NULL)
    {
    }
    void Reset(void)
    {
        pfnInit = NULL;
        pfnUpdate = NULL;
        pfnGetEntry = NULL;
        pfnMaintance = NULL;
        pfnFinial = NULL;
    }
};


