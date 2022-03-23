/*
 * =====================================================================================
 *
 *       Filename:  IWebPlugin.h
 *
 *    Description:  插件的接口定义
 *
 *        Version:  1.0
 *        Created:  2019年07月05日 23时25分18秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *   Organization:  www.quwangame.com
 *
 * =====================================================================================
 */

#ifndef _SERVERFRAME_PLUGIN_INTERFACE_
#define _SERVERFRAME_PLUGIN_INTERFACE_

#pragma once

#include "BaseCode.h"
#include <typeinfo>
#include <map>
#include "MyNet.h"
#include "IClient.h"
#include "IWebClientMgr.h"

typedef std::map< CMyString, CMyString >    STRING_MAP;
/* http 處理函數原型
 * 參數：IClient*               [IN]    pClient 
 *       http::CHttpRequest*    [IN]    http請求數據     
 *       char*                  [OUT]   返回结果
 *       uint32_t               [OUT]   返回结果的长度
 *       STRING_MAP             [OUT]   需要额外处理的http header
 * 返回值： <0      出错，需要返回错误结果
 *          =0      处理成功，无需要返回处理结果
 *          >0      处理成功，需要返回处理结果
 * */
typedef int32_t(*PCGIFUNC)(IClient*, http::CHttpRequest&, char*, uint32_t&, STRING_MAP&);

/* tcp/ws 处理函数原型
 * 参数：
 *       uint32_t     [IN]  clientId
 *       CByteStream& [IN]  传入缓冲区
 *       CByteStream& [OUT] 传出缓冲区
 * 返回值： <0      出错
 *          =0      成功
 * */
typedef int32_t (*PMSGFUNC)(uint32_t, CByteStream&, CByteStream&);

typedef std::map<CMyString, PCGIFUNC>   CGIFUNC_MAP;
typedef std::map<uint32_t, PMSGFUNC>    MSGFUNC_MAP;
struct PluginEntryData
{
    uint32_t    msgId;
    CMyString   cgiName;
    CMyString   funcName;
    PluginEntryData(uint32_t _id, const char *func)
        : msgId(_id), cgiName(), funcName(func)
    {
    }
    PluginEntryData(const char* cgi, const char *func)
        : msgId(0), cgiName(cgi), funcName(func)
    {
    }
};
typedef std::vector<PluginEntryData>    ENTRY_VECTOR;
#define REG_CGI(vec, cgi, func)     vec.push_back(PluginEntryData((cgi), (func)))
#define REG_MSG(vec, msg, func)     vec.push_back(PluginEntryData((msg), (func)))


/* plugin.so 中定义的初始化、函数列表、定期执行函数
 */
enum tagInitStyle
{
    init_pluginInit = 0,    // 初始化plugin
    init_pluginReloadCfg,   // 重新装载plugin
};
typedef int32_t (*PLUGIN_INIT)(int32_t);
typedef void (*PLUGIN_REG)(ENTRY_VECTOR&);
typedef void (*PLUGIN_RUN)(void);


// 在.h 的类中定义HTTP处理函数声明
// ENTRY_FUNC_DEFINE(doTest)
//
#define ENTRY_FUNC_DEFINE(func) int32_t func(IClient* pClient, http::CHttpRequest &req, char *szResp, uint32_t &cbResp, STRING_MAP &ext_headers);

// 在.cpp 中实现HTTP处理函数的定义
// ENTRY_FUNC_DECLARE(CTest, doTest)
// {
//  ...
// }
//
#define ENTRY_FUNC_DECLARE(CLASS, func) int32_t CLASS::func(IClient* pClient, http::CHttpRequest &req, char *szResp, uint32_t &cbResp, STRING_MAP &ext_headers) 

// 在.cpp 中声明.so的构造和析构函数
// PLUGIN_SO_INIT(CTest)
// PLUGIN_SO_FINI
//
// #define PLUGIN_SO_INIT(CLASS) __attribute ((constructor)) void _init(void) { g_pPlugin = new CLASS(); }

#define PLUGIN_SO_FINI(objptr)        __attribute ((destructor)) void _fini(void) {  SAFE_DELETE(objptr); }

// 在.cpp 中定义.so 导出函数
// ENTRY_FUNC_IMPLEMENT(doTest)
//
#define ENTRY_FUNC_IMPLEMENT(objptr, func) extern "C" int32_t func(IClient* pClient, http::CHttpRequest &req, char *szResp, uint32_t &cbResp, STRING_MAP &ext_headers) { \
    return objptr->func(pClient, req, szResp, cbResp, ext_headers); } 

#endif // _SERVERFRAME_PLUGIN_INTERFACE_


