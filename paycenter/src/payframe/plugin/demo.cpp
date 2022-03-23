/*
 * =====================================================================================
 *
 *       Filename:  demo.cpp
 *
 *    Description:  demo for plugin
 *
 *        Version:  1.0
 *        Created:  2019年09月11日 17时43分55秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "demo.h"

extern CMyString getOsInfo(void);
extern CMyString getCpuInfo(void);
extern CMyString getMemoryInfo(void);

CPluginDemo::CPluginDemo()
{

}

CPluginDemo::~CPluginDemo()
{

}

void CPluginDemo::Maintance(void)
{
    LogDebug("%s: maintance.", __func__);
}

ENTRY_FUNC_DECLARE(CPluginDemo, doEcho)
{
    strcpy(szResp, "hello,world!");
    cbResp = strlen(szResp);

    return 0;
}

ENTRY_FUNC_DECLARE(CPluginDemo, doServerInfo)
{
    CMyString osinfo = getOsInfo();
    CMyString cpuinfo = getCpuInfo();
    CMyString meminfo = getMemoryInfo();
    
    safe_sprintf(szResp, cbResp, "%s\n%s\n%s", osinfo.c_str(), cpuinfo.c_str(), meminfo.c_str());
    cbResp = strlen(szResp);

    return 0;
}

///////////////////////////////////////////////////////////////////////////
static CPluginDemo *g_pPlugin;
// PLUGIN_SO_INIT(CPluginDemo)
PLUGIN_SO_FINI

extern "C" int32_t doInit(void)
{
    // TODO: 外部传入的初始化过程
    g_pPlugin = new CPluginDemo();

    return 0;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "echo", "doEcho");
    REG_CGI(vecEntry, "serverInfo", "doServerInfo");
}

extern "C" void run(void)
{
    g_pPlugin->Maintance();
}

ENTRY_FUNC_IMPLEMENT(doEcho)
ENTRY_FUNC_IMPLEMENT(doServerInfo)


