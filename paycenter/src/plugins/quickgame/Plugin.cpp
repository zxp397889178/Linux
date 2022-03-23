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
#include "QuickGamePay.h"

CQuickGamePay *g_pQuickGamePay;
PLUGIN_SO_FINI(g_pQuickGamePay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pQuickGamePay = new CQuickGamePay();
        if (NULL  == g_pQuickGamePay)
        {
            return -1000;
        }

        ret = g_pQuickGamePay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pQuickGamePay->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "quickgame/pay",        "doDeposit");
    REG_CGI(vecEntry, "quickgame/callback",   "doDepositCB"); 
    REG_CGI(vecEntry, "quickgame/callback_hy",   "doDepositCB_hy");
}

extern "C" void run(void)
{
    g_pQuickGamePay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pQuickGamePay, doDeposit)
ENTRY_FUNC_IMPLEMENT(g_pQuickGamePay, doDepositCB)
ENTRY_FUNC_IMPLEMENT(g_pQuickGamePay, doDepositCB_hy)

