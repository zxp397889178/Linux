/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  凤凰支付SDK，菲律宾马尼拉
 *
 *        Version:  1.0
 *        Created:  2020年05月29日 12时35分51秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "Common.h"
#include "phoenix_pay.h"

CPluginPhoenixPay *g_pPhoenixPay;
PLUGIN_SO_FINI(g_pPhoenixPay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pPhoenixPay = new CPluginPhoenixPay();
        if (NULL  == g_pPhoenixPay)
        {
            return -1000;
        }

        ret = g_pPhoenixPay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pPhoenixPay->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "phoenix/pay",        "doPay");
    REG_CGI(vecEntry, "phoenix/callback",   "doCallback");
}

extern "C" void run(void)
{
    g_pPhoenixPay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pPhoenixPay, doPay)
ENTRY_FUNC_IMPLEMENT(g_pPhoenixPay, doCallback)



