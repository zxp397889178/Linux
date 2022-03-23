/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  天逸支付SDK
 *
 *        Version:  1.0
 *        Created:  2020年07月18日 19时48分49秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "Common.h"
#include "TianyiPay.h"

CPluginTianyiPay *g_pTianyiPay;
PLUGIN_SO_FINI(g_pTianyiPay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pTianyiPay = new CPluginTianyiPay();
        if (NULL  == g_pTianyiPay)
        {
            return -1000;
        }

        ret = g_pTianyiPay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pTianyiPay->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "tianyi/pay",       "doPay");
    REG_CGI(vecEntry, "tianyi/pay_scan",  "doScanPay");
    REG_CGI(vecEntry, "tianyi/callback",  "doCallback");
}

extern "C" void run(void)
{
    g_pTianyiPay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pTianyiPay, doPay)
ENTRY_FUNC_IMPLEMENT(g_pTianyiPay, doScanPay)
ENTRY_FUNC_IMPLEMENT(g_pTianyiPay, doCallback)



