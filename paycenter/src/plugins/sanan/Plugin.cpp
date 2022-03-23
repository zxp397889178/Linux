/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  三安支付
 *
 *        Version:  1.0
 *        Created:  2020年07月03日 14时15分58秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "Common.h"
#include "SananPay.h"

CPluginSananPay *g_pSananPay;
PLUGIN_SO_FINI(g_pSananPay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pSananPay = new CPluginSananPay();
        if (NULL  == g_pSananPay)
        {
            return -1000;
        }

        ret = g_pSananPay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pSananPay->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "sanan/pay",       "doPay");
    REG_CGI(vecEntry, "sanan/callback",  "doCallback");
//    REG_CGI(vecEntry, "Sanan/html",      "doGetHtml");
}

extern "C" void run(void)
{
    g_pSananPay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pSananPay, doPay)
ENTRY_FUNC_IMPLEMENT(g_pSananPay, doCallback)
// ENTRY_FUNC_IMPLEMENT(g_pSananPay, doGetHtml)





