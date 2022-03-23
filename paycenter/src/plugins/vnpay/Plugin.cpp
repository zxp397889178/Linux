/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  豪杰支付SDK
 *
 *        Version:  1.0
 *        Created:  03/15/2020 01:12:03 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "Common.h"
#include "vnpay.h"

CPluginVnPay *g_pVnPay;
PLUGIN_SO_FINI(g_pVnPay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pVnPay = new CPluginVnPay();
        if (NULL  == g_pVnPay)
        {
            return -1000;
        }

        LogWarn("---------THIS-----doInit--------%p----", g_pVnPay);

        ret = g_pVnPay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pVnPay->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "vn/pay",        "doPay");
    REG_CGI(vecEntry, "vn/callback",   "doCallback");
}

extern "C" void run(void)
{
    g_pVnPay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pVnPay, doPay)
ENTRY_FUNC_IMPLEMENT(g_pVnPay, doCallback)


