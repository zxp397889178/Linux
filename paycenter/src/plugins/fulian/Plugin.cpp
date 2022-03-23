/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  富联支付接口设计
 *
 *        Version:  1.0
 *        Created:  07/04/2020 10:07:25 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "Common.h"
#include "FulianPay.h"

CPluginFulianPay *g_pFulianPay;
PLUGIN_SO_FINI(g_pFulianPay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pFulianPay = new CPluginFulianPay();
        if (NULL  == g_pFulianPay)
        {
            return -1000;
        }

        ret = g_pFulianPay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pFulianPay->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "fulian/pay",       "doPay");
    REG_CGI(vecEntry, "fulian/callback",  "doCallback");
//    REG_CGI(vecEntry, "Fulian/html",      "doGetHtml");
}

extern "C" void run(void)
{
    g_pFulianPay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pFulianPay, doPay)
ENTRY_FUNC_IMPLEMENT(g_pFulianPay, doCallback)
// ENTRY_FUNC_IMPLEMENT(g_pFulianPay, doGetHtml)



