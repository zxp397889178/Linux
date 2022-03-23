/*
 * =====================================================================================
 *
 *       Filename:  IPlugin.h
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
#pragma once

#include "BaseCode.h"
#include "MyNet.h"
#include <typeinfo>
#include <map>
#include "PluginInfo.h"


class CClient;
/* http 處理函數原型
 * 參數：
 *       [IN]     CClient*              Client对象
 *       [IN]     http::CHttpRequest&   http请求
 *       [OUT]    char*                 返回的结果
 *       [IN/OUT] uint32_t&             char*的长度
 *       [OUT]    STRING_MAP&           额外处理的 resp header
 * 返回值： <0      出错，需要返回错误结果
 *          =0      处理成功，无需要返回处理结果
 *          >0      处理成功，需要返回处理结果
 * */
typedef std::map< CMyString, CMyString >    STRING_MAP;
typedef int32_t (*PFN_CGI)(CClient*, http::CHttpRequest&, char*, uint32_t&,STRING_MAP&);

/* tcp/ws 处理函数原型
 * 参数：
 *       [IN] CClient*          Client对象
 *       [IN] CByteStream&      数据缓冲区
 * 返回值： <0      出错
 *          =0      成功
 * */
//typedef int32_t (*PFN_MSG)(CClient*, CByteStream&);
typedef int32_t (*PFN_MSG)(CClient*, byte_t*, uint16_t);

typedef std::map<CMyString, PFN_CGI>   CGIFUNC_MAP;
typedef std::map<uint32_t, PFN_MSG>    MSGFUNC_MAP;


// 在.h 的类中定义处理函数声明
// ENTRY_FUNC_DEFINE(doTest)
//
#define ENTRY_CGI_DEFINE(func) int32_t func(CClient *pClient, http::CHttpRequest &req, char *szResp, uint32_t &cResp, STRING_MAP& ext_headers)

#define ENTRY_MSG_DEFINE(func) int32_t func(CClient *pClient, byte_t *pData, uint32_t cbData)

// 在.cpp 中实现处理函数的定义
// ENTRY_FUNC_DECLARE(CTest, doTest)
// {
//  ...
// }
//
#define ENTRY_CGI_DECLARE(CLASS, func) CLASS::func(CClient *pClient, http::CHttpRequest &req, char *szResp, uint32_t &cbResp, STRING_MAP& ext_headers)

#define ENTRY_MSG_DECLARE(CLASS, func) CLASS::func(CClient *pClient, byte_t *pData, uint32_t cbData)

// 在Plugin.cpp 中定义.so 导出函数
// ENTRY_FUNC_IMPLEMENT(doTest)
//
#define ENTRY_CGI_IMPLEMENT(objptr,func) extern "C" int32_t func(CClient *pClient, http::CHttpRequest &req, char *szResp, uint32_t &cbResp, STRING_MAP& ext_headers) {\
    if (NULL==objptr) { return -1; } \
    return objptr->func(pClient, req, szResp, cbResp, ext_headers); } 

#define ENTRY_MSG_IMPLEMENT(objptr,func) extern "C" int32_t func(CClient *pClient, byte_t* pData, uint32_t cbData) {\
    if (NULL==objptr) { return -1; } \
    return objptr->func(pClient, pData, cbData); } 


