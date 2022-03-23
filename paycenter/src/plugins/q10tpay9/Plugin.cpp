/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  Q10的支付，tpay9, https://www.tpay9.com/
 *
 *        Version:  1.0
 *        Created:  2021年06月05日 16时22分26秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "Common.h"
#include "TPay9Ex.h"

CPluginTPay9Ex    *g_pTPay9Ex;
PLUGIN_SO_FINI(g_pTPay9Ex);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pTPay9Ex = new CPluginTPay9Ex();
        if (NULL == g_pTPay9Ex)
        {
            return -1000;
        }
        return g_pTPay9Ex->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        return g_pTPay9Ex->Update();
    }
    return ret;
}


extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "tpay9/pay",              "doDeposit");
    REG_CGI(vecEntry, "tpay9/callback",         "doDepositCB");
 //   REG_CGI(vecEntry, "tpay9/deposit_query",    "doDepositQuery");
    REG_CGI(vecEntry, "tpay9/withdraw",         "doWithdraw");
    REG_CGI(vecEntry, "tpay9/withdraw_cb",      "doWithdrawCB");
}

extern "C" void run(void)
{
    g_pTPay9Ex->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pTPay9Ex, doDeposit)
ENTRY_FUNC_IMPLEMENT(g_pTPay9Ex, doDepositCB)
//ENTRY_FUNC_IMPLEMENT(g_pTPay9Ex, doDepositQuery)
ENTRY_FUNC_IMPLEMENT(g_pTPay9Ex, doWithdraw)
ENTRY_FUNC_IMPLEMENT(g_pTPay9Ex, doWithdrawCB)

