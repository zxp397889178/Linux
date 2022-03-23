/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  提现接口API
 *
 *        Version:  1.0
 *        Created:  2021年01月09日 12时01分24秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Rossen Yu (yuzp), yuzp@quwangame.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */
#include "Common.h"
#include "Withdraw.h"
#include "pay_error.h"

CPluginWithdraw *g_pWithdraw;
PLUGIN_SO_FINI(g_pWithdraw);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pWithdraw = new CPluginWithdraw();
        if (NULL  == g_pWithdraw)
        {
            return -1000;
        }

        ret = g_pWithdraw->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pWithdraw->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "withdraw/request",          "doWithdrawRequest");
    REG_CGI(vecEntry, "withdraw/audit",            "doWithdrawAudit");
    REG_CGI(vecEntry, "withdraw/credit",           "doWithdrawCredit");
}

extern "C" void run(void)
{
    g_pWithdraw->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pWithdraw, doWithdrawRequest)
ENTRY_FUNC_IMPLEMENT(g_pWithdraw, doWithdrawAudit)
ENTRY_FUNC_IMPLEMENT(g_pWithdraw, doWithdrawCredit)



