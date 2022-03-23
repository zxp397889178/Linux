/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  超凡支付平台
 *
 *        Version:  1.0
 *        Created:  03/13/2020 07:18:01 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "Common.h"
#include "chaofan_pay.h"

CPluginChaofanPay *g_pChaofanPay;
PLUGIN_SO_FINI(g_pChaofanPay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pChaofanPay = new CPluginChaofanPay();
        if (NULL  == g_pChaofanPay)
        {
            return -1000;
        }

        ret = g_pChaofanPay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pChaofanPay->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "chaofan/pay",        "doPay");
    REG_CGI(vecEntry, "chaofan/callback",   "doCallback");
    REG_CGI(vecEntry, "chanfan/html",       "doGetHtml");
}

extern "C" void run(void)
{
    g_pChaofanPay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pChaofanPay, doPay)
ENTRY_FUNC_IMPLEMENT(g_pChaofanPay, doCallback)
ENTRY_FUNC_IMPLEMENT(g_pChaofanPay, doGetHtml)


