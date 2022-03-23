/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  apizza-越南捕鱼支付SDK接入
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
#include "apizza.h"

CPluginApizza *g_pApizza;
PLUGIN_SO_FINI(g_pApizza);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pApizza = new CPluginApizza();
        if (NULL  == g_pApizza)
        {
            return -1000;
        }

        ret = g_pApizza->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pApizza->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "apizza/pay",              "doPay");
    REG_CGI(vecEntry, "apizza/callback",         "doCallback");
    REG_CGI(vecEntry, "apizza/withdraw",         "doWithdraw");
    REG_CGI(vecEntry, "apizza/withdraw_cb",      "doWithdrawCallback");
    REG_CGI(vecEntry, "apizza/withdraw_inquiry", "doWithdrawOrderInquiry");
}

extern "C" void run(void)
{
    g_pApizza->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pApizza, doPay)
ENTRY_FUNC_IMPLEMENT(g_pApizza, doCallback)
ENTRY_FUNC_IMPLEMENT(g_pApizza, doWithdraw)
ENTRY_FUNC_IMPLEMENT(g_pApizza, doWithdrawCallback)
ENTRY_FUNC_IMPLEMENT(g_pApizza, doWithdrawOrderInquiry)

