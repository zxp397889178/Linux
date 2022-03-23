/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  Quicksdk 支付
 *
 *        Version:  1.0
 *        Created:  2021年08月31日 11时35分52秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhengxianpeng (zhengxp), 397889178@qq.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */



#include "Common.h"
#include "QuickSDKPay.h"

CQuickSDKPay *g_pQuickSDKPay;
PLUGIN_SO_FINI(g_pQuickSDKPay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pQuickSDKPay = new CQuickSDKPay();
        if (NULL  == g_pQuickSDKPay)
        {
            return -1000;
        }

        ret = g_pQuickSDKPay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pQuickSDKPay->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "quicksdk/pay",        "doDeposit");
    REG_CGI(vecEntry, "quicksdk/callback_tq",   "doDepositCB"); 
    REG_CGI(vecEntry, "quicksdk/callback_hy",   "doDepositCB_hy");
}

extern "C" void run(void)
{
    g_pQuickSDKPay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pQuickSDKPay, doDeposit)
ENTRY_FUNC_IMPLEMENT(g_pQuickSDKPay, doDepositCB)
ENTRY_FUNC_IMPLEMENT(g_pQuickSDKPay, doDepositCB_hy)

