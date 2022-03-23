/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  pay77-越南捕鱼支付SDK接入
 *
 *        Version:  1.0
 *        Created:  2021年01月08日 15时10分31秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "Common.h"
#include "Pay77.h"

CPluginPay77 *g_pPay77;
PLUGIN_SO_FINI(g_pPay77);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pPay77 = new CPluginPay77();
        if (NULL  == g_pPay77)
        {
            return -1000;
        }

        ret = g_pPay77->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pPay77->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "pay77/pay",              "doPay");
    REG_CGI(vecEntry, "pay77/callback",         "doCallback");
    REG_CGI(vecEntry, "pay77/withdraw",         "doWithdraw");
    REG_CGI(vecEntry, "pay77/withdraw_cb",      "doWithdrawCallback");
    REG_CGI(vecEntry, "pay77/withdraw_inquiry", "doWithdrawOrderInquiry");
}

extern "C" void run(void)
{
    g_pPay77->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pPay77, doPay)
ENTRY_FUNC_IMPLEMENT(g_pPay77, doCallback)
ENTRY_FUNC_IMPLEMENT(g_pPay77, doWithdraw)
ENTRY_FUNC_IMPLEMENT(g_pPay77, doWithdrawCallback)
ENTRY_FUNC_IMPLEMENT(g_pPay77, doWithdrawOrderInquiry)

