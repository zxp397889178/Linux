/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  XHPay支付接口
 *
 *        Version:  1.0
 *        Created:  2020年08月08日 17时28分34秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "Common.h"
#include "XHPay.h"

CPluginXHPay *g_pXHPay;
PLUGIN_SO_FINI(g_pXHPay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pXHPay = new CPluginXHPay();
        if (NULL  == g_pXHPay)
        {
            return -1000;
        }

        ret = g_pXHPay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pXHPay->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "xhpay/pay",       "doPay");
    REG_CGI(vecEntry, "xhpay/callback",  "doCallback");
}

extern "C" void run(void)
{
    g_pXHPay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pXHPay, doPay)
ENTRY_FUNC_IMPLEMENT(g_pXHPay, doCallback)








