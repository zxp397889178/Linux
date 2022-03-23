/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  数字钱包支付
 *
 *        Version:  1.0
 *        Created:  2021年11月30日 19时16分22秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  <Your Full Name> (<Your Nick Name>), <your e-mail>
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */


#include "Common.h"
#include "DotWalletPay.h"

CDotWalletPay *g_pDotWalletPay;
PLUGIN_SO_FINI(g_pDotWalletPay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pDotWalletPay = new CDotWalletPay();
        if (NULL  == g_pDotWalletPay)
        {
            return -1000;
        }

        ret = g_pDotWalletPay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pDotWalletPay->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "dotwalletpay/pay",              "doDeposit");
    REG_CGI(vecEntry, "dotwalletpay/callback",         "doDepositCB");
    REG_CGI(vecEntry, "dotwalletpay/withdraw",         "doWithdraw");
    REG_CGI(vecEntry, "dotwalletpay/withdraw_cb",      "doWithdrawCB");
    REG_CGI(vecEntry, "dotwalletpay/getrate",          "doGetRate");
}

extern "C" void run(void)
{
    g_pDotWalletPay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pDotWalletPay, doDeposit)
ENTRY_FUNC_IMPLEMENT(g_pDotWalletPay, doDepositCB)
ENTRY_FUNC_IMPLEMENT(g_pDotWalletPay, doWithdraw)
ENTRY_FUNC_IMPLEMENT(g_pDotWalletPay, doWithdrawCB)
ENTRY_FUNC_IMPLEMENT(g_pDotWalletPay, doGetRate)

