/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  锋芒支付
 *
 *        Version:  1.0
 *        Created:  2021年06月08日 13时00分01秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "Common.h"
#include "FengmangPay.h"

CFengmangPay    *g_pFengmangPay;
PLUGIN_SO_FINI(g_pFengmangPay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-101);
    if (style == init_pluginInit)
    {
        g_pFengmangPay = new CFengmangPay();
        if (NULL == g_pFengmangPay)
        {
            return -103;
        }
        return g_pFengmangPay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        return g_pFengmangPay->Update();
    }
    return ret;
}


extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "fmpay/pay",              "doDeposit");
    REG_CGI(vecEntry, "fmpay/callback",         "doDepositCB");
    REG_CGI(vecEntry, "fmpay/withdraw",         "doWithdraw");
    REG_CGI(vecEntry, "fmpay/withdraw_cb",      "doWithdrawCB");
}

extern "C" void run(void)
{
    g_pFengmangPay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pFengmangPay, doDeposit)
ENTRY_FUNC_IMPLEMENT(g_pFengmangPay, doDepositCB)
ENTRY_FUNC_IMPLEMENT(g_pFengmangPay, doWithdraw)
ENTRY_FUNC_IMPLEMENT(g_pFengmangPay, doWithdrawCB)



