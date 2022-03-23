/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  博泰支付SDK
 *
 *        Version:  1.0
 *        Created:  07/04/2020 10:59:19 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "Common.h"
#include "BotaiPay.h"

CPluginBotaiPay *g_pBotaiPay;
PLUGIN_SO_FINI(g_pBotaiPay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pBotaiPay = new CPluginBotaiPay();
        if (NULL  == g_pBotaiPay)
        {
            return -1000;
        }

        ret = g_pBotaiPay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pBotaiPay->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "botai/pay",       "doPay");
    REG_CGI(vecEntry, "botai/callback",  "doCallback");
//    REG_CGI(vecEntry, "botai/html",      "doGetHtml");
}

extern "C" void run(void)
{
    g_pBotaiPay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pBotaiPay, doPay)
ENTRY_FUNC_IMPLEMENT(g_pBotaiPay, doCallback)
// ENTRY_FUNC_IMPLEMENT(g_pBotaiPay, doGetHtml)


