/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  Q9捕鱼，支付SDK接入
 *
 *        Version:  1.0
 *        Created:  2021年06月16日 00时10分25秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "Common.h"
#include "Q8Pay.h"

CPluginQ8Pay *g_pQ8Pay;
PLUGIN_SO_FINI(g_pQ8Pay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pQ8Pay = new CPluginQ8Pay();
        if (NULL  == g_pQ8Pay)
        {
            return -1000;
        }

        ret = g_pQ8Pay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pQ8Pay->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "q8pay/pay",          "doDeposit");
    REG_CGI(vecEntry, "q8pay/callback",       "doDepositCB");
    REG_CGI(vecEntry, "q8pay/withdraw",         "doWithdraw");
    REG_CGI(vecEntry, "q8pay/withdraw_cb",      "doWithdrawCB");
}

extern "C" void run(void)
{
    g_pQ8Pay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pQ8Pay, doDeposit)
ENTRY_FUNC_IMPLEMENT(g_pQ8Pay, doDepositCB)
ENTRY_FUNC_IMPLEMENT(g_pQ8Pay, doWithdraw)
ENTRY_FUNC_IMPLEMENT(g_pQ8Pay, doWithdrawCB)



