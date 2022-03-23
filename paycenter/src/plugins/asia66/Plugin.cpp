/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  亚洲娱乐的支付接入
 *
 *        Version:  1.0
 *        Created:  2020年05月16日 20时12分19秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "Common.h"
#include "asia66_pay.h"

CPluginAsia66Pay *g_pAsia66Pay;
PLUGIN_SO_FINI(g_pAsia66Pay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pAsia66Pay = new CPluginAsia66Pay();
        if (NULL  == g_pAsia66Pay)
        {
            return -1000;
        }

        ret = g_pAsia66Pay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pAsia66Pay->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "asia66/pay",        "doPay");
    REG_CGI(vecEntry, "asia66/callback",   "doCallback");
}

extern "C" void run(void)
{
    g_pAsia66Pay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pAsia66Pay, doPay)
ENTRY_FUNC_IMPLEMENT(g_pAsia66Pay, doCallback)



