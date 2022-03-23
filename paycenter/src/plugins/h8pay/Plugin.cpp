/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  H8捕鱼 支付SDK接入
 *
 *        Version:  1.0
 *        Created:  2021年06月13日 19时46分04秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "Common.h"
#include "H8Pay.h"

CPluginH8Pay *g_pH8Pay;
PLUGIN_SO_FINI(g_pH8Pay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pH8Pay = new CPluginH8Pay();
        if (NULL  == g_pH8Pay)
        {
            return -1000;
        }

        ret = g_pH8Pay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pH8Pay->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "h8pay/deposit",          "doDeposit");
    REG_CGI(vecEntry, "h8pay/deposit_cb",       "doDepositCB");
    REG_CGI(vecEntry, "h8pay/withdraw",         "doWithdraw");
    REG_CGI(vecEntry, "h8pay/withdraw_cb",      "doWithdrawCB");
}

extern "C" void run(void)
{
    g_pH8Pay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pH8Pay, doDeposit)
ENTRY_FUNC_IMPLEMENT(g_pH8Pay, doDepositCB)
ENTRY_FUNC_IMPLEMENT(g_pH8Pay, doWithdraw)
ENTRY_FUNC_IMPLEMENT(g_pH8Pay, doWithdrawCB)


