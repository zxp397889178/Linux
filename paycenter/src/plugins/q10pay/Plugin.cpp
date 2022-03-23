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
#include "Q10Pay.h"

CPluginQ10Pay *g_pQ10Pay;
PLUGIN_SO_FINI(g_pQ10Pay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pQ10Pay = new CPluginQ10Pay();
        if (NULL  == g_pQ10Pay)
        {
            return -1000;
        }

        ret = g_pQ10Pay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pQ10Pay->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "q10pay/pay",              "doDeposit");
    REG_CGI(vecEntry, "q10pay/callback",         "doDepositCB");
    REG_CGI(vecEntry, "q10pay/withdraw",         "doWithdraw");
    REG_CGI(vecEntry, "q10pay/withdraw_cb",      "doWithdrawCB");
//    REG_CGI(vecEntry, "q10pay/withdraw_inquiry", "doWithdrawOrderInquiry");
}

extern "C" void run(void)
{
    g_pQ10Pay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pQ10Pay, doDeposit)
ENTRY_FUNC_IMPLEMENT(g_pQ10Pay, doDepositCB)
ENTRY_FUNC_IMPLEMENT(g_pQ10Pay, doWithdraw)
ENTRY_FUNC_IMPLEMENT(g_pQ10Pay, doWithdrawCB)
//ENTRY_FUNC_IMPLEMENT(g_pQ10Pay, doWithdrawOrderInquiry)

