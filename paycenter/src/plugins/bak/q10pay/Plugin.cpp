/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  q10pay-越南捕鱼支付SDK接入
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
#include "q10pay.h"

CPluginQ10pay *g_pQ10pay;
PLUGIN_SO_FINI(g_pQ10pay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pQ10pay = new CPluginQ10pay();
        if (NULL  == g_pQ10pay)
        {
            return -1000;
        }

        ret = g_pQ10pay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pQ10pay->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "q10pay/pay",              "doPay");
    REG_CGI(vecEntry, "q10pay/callback",         "doCallback");
    REG_CGI(vecEntry, "q10pay/withdraw",         "doWithdraw");
    REG_CGI(vecEntry, "q10pay/withdraw_cb",      "doWithdrawCallback");
//    REG_CGI(vecEntry, "q10pay/withdraw_inquiry", "doWithdrawOrderInquiry");
}

extern "C" void run(void)
{
    g_pQ10pay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pQ10pay, doPay)
ENTRY_FUNC_IMPLEMENT(g_pQ10pay, doCallback)
ENTRY_FUNC_IMPLEMENT(g_pQ10pay, doWithdraw)
ENTRY_FUNC_IMPLEMENT(g_pQ10pay, doWithdrawCallback)
//ENTRY_FUNC_IMPLEMENT(g_pQ10pay, doWithdrawOrderInquiry)

