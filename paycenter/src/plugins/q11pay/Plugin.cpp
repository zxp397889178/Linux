/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  q11pay-锋芒支付sdk接入
 *
 *        Version:  1.0
 *        Created:  2021年07月02日 16时42分31秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhengxianpeng (zxp), zhengxp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "Common.h"
#include "Q11Pay.h"

CPluginQ11Pay    *g_pQ11Pay;
PLUGIN_SO_FINI(g_pQ11Pay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pQ11Pay = new CPluginQ11Pay();
        if (NULL == g_pQ11Pay)
        {
            return -1000;
        }
        return g_pQ11Pay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        return g_pQ11Pay->Update();
    }
    return ret;
}


extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "q11pay/pay",              "doDeposit");
    REG_CGI(vecEntry, "q11pay/callback",         "doDepositCB");
    REG_CGI(vecEntry, "q11pay/withdraw",         "doWithdraw");
    REG_CGI(vecEntry, "q11pay/withdraw_cb",      "doWithdrawCB");
}

extern "C" void run(void)
{
    g_pQ11Pay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pQ11Pay, doDeposit)
ENTRY_FUNC_IMPLEMENT(g_pQ11Pay, doDepositCB)
ENTRY_FUNC_IMPLEMENT(g_pQ11Pay, doWithdraw)
ENTRY_FUNC_IMPLEMENT(g_pQ11Pay, doWithdrawCB)
