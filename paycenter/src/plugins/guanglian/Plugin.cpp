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
#include "GuangLianPay.h"

CPluginGuangLianPay *g_pGuangLianPay;
PLUGIN_SO_FINI(g_pGuangLianPay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pGuangLianPay = new CPluginGuangLianPay();
        if (NULL  == g_pGuangLianPay)
        {
            return -1000;
        }

        ret = g_pGuangLianPay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pGuangLianPay->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "guanglian/pay",       "doPay");
    REG_CGI(vecEntry, "guanglian/callback",  "doCallback");
}

extern "C" void run(void)
{
    g_pGuangLianPay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pGuangLianPay, doPay)
ENTRY_FUNC_IMPLEMENT(g_pGuangLianPay, doCallback)


