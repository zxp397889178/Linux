/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  FUN 支付接口
 *
 *        Version:  1.0
 *        Created:  2020年07月01日 16时05分35秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "Common.h"
#include "FunPay.h"

CPluginFunPay *g_pFunPay;
PLUGIN_SO_FINI(g_pFunPay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pFunPay = new CPluginFunPay();
        if (NULL  == g_pFunPay)
        {
            return -1000;
        }

        ret = g_pFunPay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pFunPay->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "fun/pay",       "doPay");
    REG_CGI(vecEntry, "fun/callback",  "doCallback");
//    REG_CGI(vecEntry, "fun/html",      "doGetHtml");
}

extern "C" void run(void)
{
    g_pFunPay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pFunPay, doPay)
ENTRY_FUNC_IMPLEMENT(g_pFunPay, doCallback)
// ENTRY_FUNC_IMPLEMENT(g_pFunPay, doGetHtml)



