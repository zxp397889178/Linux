/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  豪杰支付SDK
 *
 *        Version:  1.0
 *        Created:  03/15/2020 01:12:03 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "IvnPay.h"
#include "Common.h"

CPluginIvnPay *g_pIvnPay;
PLUGIN_SO_FINI(g_pIvnPay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pIvnPay = new CPluginIvnPay();
        if (NULL  == g_pIvnPay)
        {
            return -1000;
        }
        ret = g_pIvnPay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pIvnPay->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "ivnpay/pay",        "doDeposit"); 
    REG_CGI(vecEntry, "ivnpay/callback",   "doDepositCB");
    REG_CGI(vecEntry, "ivnpay/withdraw",   "doWithdraw");
    REG_CGI(vecEntry, "ivnpay/withdraw_cb","doWithdrawCB");
}

extern "C" void run(void)
{
    g_pIvnPay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pIvnPay, doDeposit)
ENTRY_FUNC_IMPLEMENT(g_pIvnPay, doDepositCB)
ENTRY_FUNC_IMPLEMENT(g_pIvnPay, doWithdraw)
ENTRY_FUNC_IMPLEMENT(g_pIvnPay, doWithdrawCB)

