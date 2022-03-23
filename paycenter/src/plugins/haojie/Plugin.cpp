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
#include "haojie_pay.h"

CPluginHaojiePay *g_pHaojiePay;
PLUGIN_SO_FINI(g_pHaojiePay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pHaojiePay = new CPluginHaojiePay();
        if (NULL  == g_pHaojiePay)
        {
            return -1000;
        }

        ret = g_pHaojiePay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pHaojiePay->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "haojie/pay",        "doPay");
    REG_CGI(vecEntry, "haojie/callback",   "doCallback");
}

extern "C" void run(void)
{
    g_pHaojiePay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pHaojiePay, doPay)
ENTRY_FUNC_IMPLEMENT(g_pHaojiePay, doCallback)


