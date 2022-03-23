/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  天下汇支付SDK
 *
 *        Version:  1.0
 *        Created:  2020年05月29日 13时08分58秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "Common.h"
#include "tianxiahui_pay.h"

CPluginTianxiahuiPay *g_pTianxiahuiPay;
PLUGIN_SO_FINI(g_pTianxiahuiPay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pTianxiahuiPay = new CPluginTianxiahuiPay();
        if (NULL  == g_pTianxiahuiPay)
        {
            return -1000;
        }

        ret = g_pTianxiahuiPay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pTianxiahuiPay->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "tianxiahui/pay",        "doPay");
    REG_CGI(vecEntry, "tianxiahui/callback",   "doCallback");
}

extern "C" void run(void)
{
    g_pTianxiahuiPay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pTianxiahuiPay, doPay)
ENTRY_FUNC_IMPLEMENT(g_pTianxiahuiPay, doCallback)

