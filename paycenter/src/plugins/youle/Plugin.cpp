/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  有乐支付SDK
 *
 *        Version:  1.0
 *        Created:  2020年06月30日 00时03分00秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "Common.h"
#include "YoulePay.h"

CPluginYoulePay *g_pYoulePay;
PLUGIN_SO_FINI(g_pYoulePay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pYoulePay = new CPluginYoulePay();
        if (NULL  == g_pYoulePay)
        {
            return -1000;
        }

        ret = g_pYoulePay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pYoulePay->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "youle/pay",        "doPay");
    REG_CGI(vecEntry, "youle/callback",   "doCallback");
}

extern "C" void run(void)
{
    g_pYoulePay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pYoulePay, doPay)
ENTRY_FUNC_IMPLEMENT(g_pYoulePay, doCallback)

