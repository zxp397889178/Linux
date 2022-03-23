/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  龙77pay 支付
 *
 *        Version:  1.0
 *        Created:  2021年08月04日 17时48分32秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhengxianpeng (zhengxp), 397889178@qq.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */

#include "Common.h"
#include "Long77Pay.h"

CLong77Pay    *g_pLong77Pay;
PLUGIN_SO_FINI(g_pLong77Pay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-101);
    if (style == init_pluginInit)
    {
        g_pLong77Pay = new CLong77Pay();
        if (NULL == g_pLong77Pay)
        {
            return -103;
        }
        return g_pLong77Pay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        return g_pLong77Pay->Update();
    }
    return ret;
}


extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "long77/pay",              "doDeposit");
    REG_CGI(vecEntry, "long77/callback",         "doDepositCB");
    REG_CGI(vecEntry, "long77/withdraw",         "doWithdraw");
    REG_CGI(vecEntry, "long77/withdraw_cb",      "doWithdrawCB");
}

extern "C" void run(void)
{
    g_pLong77Pay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pLong77Pay, doDeposit)
ENTRY_FUNC_IMPLEMENT(g_pLong77Pay, doDepositCB)
ENTRY_FUNC_IMPLEMENT(g_pLong77Pay, doWithdraw)
ENTRY_FUNC_IMPLEMENT(g_pLong77Pay, doWithdrawCB)



