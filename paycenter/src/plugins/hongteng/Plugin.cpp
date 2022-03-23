/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  鸿腾支付SDK 
 *                  (http://zf.qieziyy.com/doc.html) 
 *        Version:  1.0
 *        Created:  2020年05月28日 21时22分28秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "Common.h"
#include "hongteng_pay.h"

CPluginHongTengPay *g_pHongTengPay;
PLUGIN_SO_FINI(g_pHongTengPay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pHongTengPay = new CPluginHongTengPay();
        if (NULL  == g_pHongTengPay)
        {
            return -1000;
        }

        ret = g_pHongTengPay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pHongTengPay->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "hongteng/pay",        "doPay");
    REG_CGI(vecEntry, "hongteng/callback",   "doCallback");
}

extern "C" void run(void)
{
    g_pHongTengPay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pHongTengPay, doPay)
ENTRY_FUNC_IMPLEMENT(g_pHongTengPay, doCallback)


