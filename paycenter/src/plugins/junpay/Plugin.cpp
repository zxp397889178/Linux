/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  俊支付
 *
 *        Version:  1.0
 *        Created:  2020年07月17日 22时53分35秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "Common.h"
#include "JunPay.h"

CPluginJunPay *g_pJunPay;
PLUGIN_SO_FINI(g_pJunPay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pJunPay = new CPluginJunPay();
        if (NULL  == g_pJunPay)
        {
            return -1000;
        }

        ret = g_pJunPay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pJunPay->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "jun/pay",       "doPay");
    REG_CGI(vecEntry, "jun/callback",  "doCallback");
}

extern "C" void run(void)
{
    g_pJunPay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pJunPay, doPay)
ENTRY_FUNC_IMPLEMENT(g_pJunPay, doCallback)


