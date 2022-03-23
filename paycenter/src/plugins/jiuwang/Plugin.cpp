/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  九网支付API接口
 *
 *        Version:  1.0
 *        Created:  2020年07月29日 17时18分45秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "Common.h"
#include "JiuwangPay.h"

CPluginJiuwangPay *g_pJiuwangPay;
PLUGIN_SO_FINI(g_pJiuwangPay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pJiuwangPay = new CPluginJiuwangPay();
        if (NULL  == g_pJiuwangPay)
        {
            return -1000;
        }

        ret = g_pJiuwangPay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pJiuwangPay->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "jiuwang/pay",       "doPay");
    REG_CGI(vecEntry, "jiuwang/callback",  "doCallback");
}

extern "C" void run(void)
{
    g_pJiuwangPay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pJiuwangPay, doPay)
ENTRY_FUNC_IMPLEMENT(g_pJiuwangPay, doCallback)





