/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  SVIP的支付接口
 *
 *        Version:  1.0
 *        Created:  2020年06月30日 14时54分49秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "Common.h"
#include "SVipPay.h"

CPluginSVipPay *g_pSVipPay;
PLUGIN_SO_FINI(g_pSVipPay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pSVipPay = new CPluginSVipPay();
        if (NULL  == g_pSVipPay)
        {
            return -1000;
        }

        ret = g_pSVipPay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pSVipPay->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "svip/pay",       "doPay");
    REG_CGI(vecEntry, "svip/callback",  "doCallback");
    REG_CGI(vecEntry, "svip/html",      "doGetHtml");
}

extern "C" void run(void)
{
    g_pSVipPay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pSVipPay, doPay)
ENTRY_FUNC_IMPLEMENT(g_pSVipPay, doCallback)
ENTRY_FUNC_IMPLEMENT(g_pSVipPay, doGetHtml)


