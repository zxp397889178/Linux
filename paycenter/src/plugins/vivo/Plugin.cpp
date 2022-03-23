/*
 * =====================================================================================
 *
 *       Filename:  Plugin.cpp
 *
 *    Description:  VIVO支付渠道
 *
 *        Version:  1.0
 *        Created:  2021年08月16日 17时49分23秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  zhengxianpeng (zhengxp), 397889178@qq.com
 *        Company:  www.quwangame.com
 *
 * =====================================================================================
 */



#include "Common.h"
#include "VivoPay.h"

CVivoPay *g_pVivoPay;
PLUGIN_SO_FINI(g_pVivoPay);

extern "C" int32_t doInit(int32_t style)
{
    int32_t ret(-1001);
    if (style == init_pluginInit)
    {
        g_pVivoPay = new CVivoPay();
        if (NULL  == g_pVivoPay)
        {
            return -1000;
        }

        ret = g_pVivoPay->Init();
    }
    else if (style == init_pluginReloadCfg)
    {
        ret = g_pVivoPay->Update();
    }

    return ret;
}

extern "C" void queryRegister(ENTRY_VECTOR &vecEntry)
{
    REG_CGI(vecEntry, "vivo/pay",        "doDeposit");
    REG_CGI(vecEntry, "vivo/sign",       "doGetSign");
    REG_CGI(vecEntry, "vivo/callback",   "doDepositCB");
}

extern "C" void run(void)
{
    g_pVivoPay->Maintance();
}

ENTRY_FUNC_IMPLEMENT(g_pVivoPay, doDeposit)
ENTRY_FUNC_IMPLEMENT(g_pVivoPay, doGetSign)
ENTRY_FUNC_IMPLEMENT(g_pVivoPay, doDepositCB)
